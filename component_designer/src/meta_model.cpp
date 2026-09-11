/********************************************************************************
 * Copyright (c) 2025-2026 ZF Friedrichshafen AG
 * 
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Contributors:
 *   Thomas Pfleiderer - initial API and implementation
 ********************************************************************************/
 
#include "meta_model.h"

#include <algorithm>

#include "yaml_parser.h"

namespace acd {
namespace {

const std::vector<std::string>& EmptyStrings() {
    static const std::vector<std::string> empty;
    return empty;
}

bool ToBool(const std::string& text) { return text == "true" || text == "True" || text == "yes"; }

} // namespace

const MetaProperty* MetaInterfaceType::FindProperty(const std::string& propertyName) const {
    const auto it = std::find_if(properties.begin(), properties.end(),
                                 [&propertyName](const MetaProperty& p) { return p.name == propertyName; });
    return it == properties.end() ? nullptr : &(*it);
}

bool MetaModel::Load(const std::string& path, std::string& error) {
    YamlParser parser;
    const YamlNodePtr root = parser.ParseFile(path, error);
    if (!root) {
        return false;
    }
    const YamlNodePtr model = root->Find("metamodel");
    if (!model || !model->IsMap()) {
        error = "'metamodel' root key not found in " + path;
        return false;
    }

    m_interfaceTypes.clear();
    m_enums.clear();
    m_name = model->ScalarOf("name");
    m_version = model->ScalarOf("version");
    m_sourcePath = path;

    if (const YamlNodePtr enums = model->Find("enums")) {
        for (const auto& entry : enums->GetMap()) {
            std::vector<std::string> values;
            if (entry.second && entry.second->IsSequence()) {
                for (const YamlNodePtr& value : entry.second->GetSequence()) {
                    if (value && value->IsScalar()) {
                        values.push_back(value->GetScalar());
                    }
                }
            }
            m_enums.emplace(entry.first, std::move(values));
        }
    }

    if (const YamlNodePtr types = model->Find("interfaceTypes")) {
        for (const auto& entry : types->GetMap()) {
            MetaInterfaceType type;
            type.name = entry.first;
            if (entry.second && entry.second->IsMap()) {
                type.description = entry.second->ScalarOf("description");
                if (const YamlNodePtr properties = entry.second->Find("properties")) {
                    for (const auto& property : properties->GetMap()) {
                        MetaProperty meta;
                        meta.name = property.first;
                        if (property.second && property.second->IsMap()) {
                            meta.dataType = property.second->ScalarOf("dataType");
                            meta.enumRef = property.second->ScalarOf("enumRef");
                            meta.description = property.second->ScalarOf("description");
                            meta.mandatory = ToBool(property.second->ScalarOf("mandatory"));
                        }
                        type.properties.push_back(std::move(meta));
                    }
                }
            }
            m_interfaceTypes.push_back(std::move(type));
        }
    }

    m_loaded = true;
    return true;
}

const MetaInterfaceType* MetaModel::FindInterfaceType(const std::string& name) const {
    const auto it = std::find_if(m_interfaceTypes.begin(), m_interfaceTypes.end(),
                                 [&name](const MetaInterfaceType& t) { return t.name == name; });
    return it == m_interfaceTypes.end() ? nullptr : &(*it);
}

const std::vector<std::string>& MetaModel::EnumValues(const std::string& name) const {
    const auto it = m_enums.find(name);
    return it == m_enums.end() ? EmptyStrings() : it->second;
}

std::vector<std::string> MetaModel::ColumnsFor(const std::string& interfaceTypeName,
                                               const std::vector<YamlNodePtr>& entries) const {
    std::vector<std::string> columns;
    if (const MetaInterfaceType* type = FindInterfaceType(interfaceTypeName)) {
        for (const MetaProperty& property : type->properties) {
            columns.push_back(property.name);
        }
    }
    for (const YamlNodePtr& entry : entries) {
        if (!entry || !entry->IsMap()) {
            continue;
        }
        for (const auto& field : entry->GetMap()) {
            if (std::find(columns.begin(), columns.end(), field.first) == columns.end()) {
                columns.push_back(field.first);
            }
        }
    }
    return columns;
}

} // namespace acd

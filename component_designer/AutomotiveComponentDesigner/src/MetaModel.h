// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

#ifndef ACD_METAMODEL_H
#define ACD_METAMODEL_H

#include <map>
#include <string>
#include <vector>

#include "YamlNode.h"

namespace acd {

/// One property of an interface type as declared by the meta model.
struct MetaProperty {
    std::string name;
    std::string dataType;
    std::string enumRef;
    std::string description;
    bool mandatory = false;
};

/// One interface type (Data, Parameter, Scheduling, ...) of the meta model.
struct MetaInterfaceType {
    std::string name;
    std::string description;
    std::vector<MetaProperty> properties;

    const MetaProperty* FindProperty(const std::string& name) const;
};

/// In-memory representation of autoapiframework_meta_model.yaml.
class MetaModel {
public:
    /// Loads the meta model from @p path. Returns false and fills @p error on failure.
    bool Load(const std::string& path, std::string& error);
    bool IsLoaded() const { return m_loaded; }

    const std::string& GetName() const { return m_name; }
    const std::string& GetVersion() const { return m_version; }
    const std::string& GetSourcePath() const { return m_sourcePath; }

    const MetaInterfaceType* FindInterfaceType(const std::string& name) const;
    /// Allowed values of the enumeration @p name, empty if unknown.
    const std::vector<std::string>& EnumValues(const std::string& name) const;

    /// Column order for a section: meta model properties first, followed by
    /// any additional keys that occur in @p entries but are not in the model.
    std::vector<std::string> ColumnsFor(const std::string& interfaceTypeName,
                                        const std::vector<YamlNodePtr>& entries) const;

private:
    bool m_loaded = false;
    std::string m_name;
    std::string m_version;
    std::string m_sourcePath;
    std::vector<MetaInterfaceType> m_interfaceTypes;
    std::map<std::string, std::vector<std::string>> m_enums;
};

} // namespace acd

#endif // ACD_METAMODEL_H

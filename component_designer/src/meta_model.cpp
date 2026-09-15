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

namespace acd 
{
    bool MetaModel::Load(const std::string& path, std::string& error) 
    {
        YamlParser parser;
        const YamlNodePtr root = parser.ParseFile(path, error);
        if (!root) 
        {
            return false;
        }
        const YamlNodePtr model = root->Find(kMetaModelKey);
        if (!model || !model->IsMap()) 
        {
            error = std::string("'") + std::string(kMetaModelKey) + "' root key not found in " + path;
            return false;
        }
        
        m_interfaceTypes.clear();        
        if (!FindRequiredAttributesAndInterfaces(model, path, error))
        {
            m_interfaceTypes.clear();            
            return false;
        }

        m_sourcePath = path;

        m_fileContent = parser.GetFileContent(path); // Initialize the file content to an empty string
        m_loaded = true;
        return true;
    }

    bool MetaModel::FindRequiredAttributesAndInterfaces(const YamlNodePtr model, 
                    const std::string& path,std::string& error)
    {
        m_name = model->ScalarOf(kNameKey);
        m_version = model->ScalarOf(kVersionKey);
        if (m_name.compare(kExpectedName) != 0 ||
            m_version.compare("") == 0)
        {
            error = "Incompatible metamodel file " + path;
            return false;
        }

        bool interfacesNotFound = true;        
        const YamlNodePtr interfaceTypes = model->Find(kInterfaceTypesKey);
        if (interfaceTypes && interfaceTypes->IsMap()) 
        {
            const YamlNodePtr dataInterfaces = interfaceTypes->Find(kDataInterfaceTypeKey);
            const YamlNodePtr parameters = interfaceTypes->Find(kParameterInterfaceTypeKey);
            const YamlNodePtr scheduling = interfaceTypes->Find(kSchedulingInterfaceTypeKey);
            if (dataInterfaces && parameters && scheduling &&
                dataInterfaces->IsMap() && parameters->IsMap() && scheduling->IsMap())
            {
                const YamlNodePtr dataProperties = dataInterfaces->Find(kPropertiesKey);
                const YamlNodePtr parametersProperties = parameters->Find(kPropertiesKey);
                const YamlNodePtr schedulingParameters = scheduling->Find(kPropertiesKey);  
                if (dataProperties && parameters && schedulingParameters  &&
                    dataProperties->IsMap() && parametersProperties->IsMap() && schedulingParameters->IsMap())
                {
                    interfacesNotFound = false;                
                }
            }
            m_interfaceTypes.push_back(std::move(dataInterfaces));
            m_interfaceTypes.push_back(std::move(parameters));
            m_interfaceTypes.push_back(std::move(scheduling));            
        }

        if (interfacesNotFound)
        {
            error = "The required interface types are missing in the metamodel file " + path;
            return false;
        }
        return true;
    }    

    const YamlNodePtr* MetaModel::FindInterfaceType(const std::string& name) const 
    {
        static constexpr const char* interfaceTypeNames[] = {
            kDataInterfaceTypeKey,
            kParameterInterfaceTypeKey,
            kSchedulingInterfaceTypeKey
        };

        for (std::size_t index = 0; index < m_interfaceTypes.size() &&
             index < std::size(interfaceTypeNames); ++index)
        {
            if (name == interfaceTypeNames[index])
            {
                return &m_interfaceTypes[index];
            }
        }

        return nullptr;
    }

    std::vector<std::string> MetaModel::ColumnsFor(const std::string& interfaceTypeName,
                                                const std::vector<YamlNodePtr>& entries) const 
    {
        std::vector<std::string> columns;
        if (const YamlNodePtr* type = FindInterfaceType(interfaceTypeName)) 
        {
            const YamlNodePtr properties = (*type)->Find(kPropertiesKey);
            if (properties && properties->IsMap())
            {
                for (const auto& property : properties->GetMap())
                {
                    columns.push_back(property.first);
                }
            }
        }
        for (const YamlNodePtr& entry : entries) 
        {
            if (!entry || !entry->IsMap())
            {
                continue;
            }
            for (const auto& field : entry->GetMap()) 
            {
                if (std::find(columns.begin(), columns.end(), field.first) == columns.end()) 
                {
                    columns.push_back(field.first);
                }
            }
        }
        return columns;
    }

} // namespace acd

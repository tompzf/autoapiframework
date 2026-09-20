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
 
#include "validate_function.h"

#include "meta_model.h"
#include "function_specification.h"

#include <cstdint>
#include <algorithm>

#include "yaml_parser.h"

namespace acd 
{
    bool ValidateFunction::SyntaxCheckIsOK(const MetaModel& metaModel, const std::string& content, std::string& error)
    {
        YamlParser parser;
        const YamlNodePtr root = parser.ParseText(content, error);
        if (!root)
        {
            return false;
        }
        const YamlNodePtr functionSpecification = root->Find(FunctionSpecification::kRootKey);
        if (!functionSpecification || !functionSpecification->IsMap())
        {
            error = std::string("'") + std::string(FunctionSpecification::kRootKey) + "' root key not found.";
            return false;
        }

        if (!HeaderSyntaxCheck(functionSpecification, error))
        {
            return false;
        }

        if (!InterfaceTypesSyntaxCheck(functionSpecification, metaModel, error))
        {
            return false;
        }

        return true;
    }

    bool ValidateFunction::HeaderSyntaxCheck(const YamlNodePtr& root, std::string& error)
    {
        auto name = root->ScalarOf(FunctionSpecification::kNameKey);
        auto version = root->ScalarOf(FunctionSpecification::kVersionKey);
     
        if (name.size() == 0 || version.size() == 0)
        {
            error = std::string(FunctionSpecification::kNameKey) + " or " + std::string(FunctionSpecification::kVersionKey) 
                    + std::string(" in the function specification not found.");  
            return false;
        }        
        return true;
    }

    bool ValidateFunction::InterfaceTypesSyntaxCheck(const YamlNodePtr& root, const MetaModel& metaModel, std::string& error)
    {
        const YamlNodePtr dataInterfaces = root->Find(FunctionSpecification::kDataInterfacesKey);
        const YamlNodePtr parameters = root->Find(FunctionSpecification::kParametersKey);
        const YamlNodePtr scheduling = root->Find(FunctionSpecification::kSchedulingKey);   
        if (!dataInterfaces || !parameters || !scheduling) 
        {
            error = std::string(FunctionSpecification::kDataInterfacesKey) + " or " + std::string(FunctionSpecification::kParametersKey) 
                    + " or " + std::string(FunctionSpecification::kSchedulingKey) + " not found.";
            return false;
        }

        if (!SyntaxCheckForSequence(dataInterfaces, FunctionSpecification::kNamePathKey, error, FunctionSpecification::kDataInterfacesKey)) 
        {
            return false;
        }
        // if (!ValidateProperties(dataInterfaces, acd::kDataInterfaceTypeKey, metaModel, error))
        // {
        //     return false;
        // }
        if (!ValidateEnums(dataInterfaces, acd::kDataInterfaceTypeKey, metaModel, error))
        {
            return false;
        }        

        if (!SyntaxCheckForSequence(parameters, FunctionSpecification::kNamePathKey, error, FunctionSpecification::kParametersKey)) 
        {
            return false;
        }
        // if (!ValidateProperties(parameters, acd::kParameterInterfaceTypeKey, metaModel, error))
        // {
        //     return false;
        // }
        if (!ValidateEnums(parameters, acd::kParameterInterfaceTypeKey, metaModel, error))
        {
            return false;
        }          

        if (!SyntaxCheckForSequence(scheduling, FunctionSpecification::kFunctionNameKey, error, FunctionSpecification::kSchedulingKey)) 
        {
            return false;
        }
    
        return true;
    }

    bool ValidateFunction::SyntaxCheckForSequence(const YamlNodePtr& node, const std::string& key, std::string& error, const std::string& displayName)
    {
        for (const YamlNodePtr& dataInterface : node->GetSequence())
        {
            const YamlNodePtr namePath = dataInterface
                ? dataInterface->Find(key)
                : nullptr;
            if (!dataInterface || !dataInterface->IsMap() || !namePath || !namePath->IsScalar() || namePath->GetScalar().empty())
            {
                error = "Each " + displayName + " must be a mapping with a non-empty " + key + ".";
                return false;
            }
        }        
        return true;
    }    

    bool ValidateFunction::ValidateProperties(const YamlNodePtr& dataInterfaces, const std::string& interfaceTypeName,
                                            const MetaModel& metaModel, std::string& error)
    {
        std::vector<std::string> validPropertyNamesFromMetaModel;
        if (const YamlNodePtr* type = metaModel.FindInterfaceType(interfaceTypeName))
        {
            const YamlNodePtr properties = (*type)->Find(kPropertiesKey);
            if (properties && properties->IsMap())
            {
                for (const auto& property : properties->GetMap())
                {
                    validPropertyNamesFromMetaModel.push_back(property.first);
                }
            }
        }

        bool syntaxError = false;
        std::string text = "";
        for (const YamlNodePtr& item : dataInterfaces->GetSequence())
        {
            if (!item || !item->IsMap())
            {
                error = "Each collection item must be a mapping.";
                return false;
            }
            for (const auto& property : item->GetMap())
            {
                auto it = std::find(validPropertyNamesFromMetaModel.begin(), 
                                    validPropertyNamesFromMetaModel.end(), property.first);
                if (it == validPropertyNamesFromMetaModel.end())
                {
                    syntaxError = true;
                    text += " " + property.first;
                }
            }
        }

        if (syntaxError)
        {
            error = "Invalid properties found: " + text;
            return false;
        }
        return true;
    }

    bool ValidateFunction::ValidateEnums(const YamlNodePtr& dataInterfaces, const std::string& interfaceTypeName,
                                         const MetaModel& metaModel, std::string& error)
    {
        std::map<std::string, std::string> propertiesWithEnumRef;
        if (const YamlNodePtr* type = metaModel.FindInterfaceType(interfaceTypeName))
        {
            const YamlNodePtr properties = (*type)->Find(kPropertiesKey);
            if (properties && properties->IsMap())
            {
                for (const auto& property : properties->GetMap())                
                {
                    const YamlNodePtr enumRef = property.second->Find(kEnumRefKey);
                    if (enumRef && enumRef->IsScalar())
                    {
                        propertiesWithEnumRef.insert(std::make_pair(property.first, enumRef->GetScalar()));
                    }
                }
            }
        }

        for (const YamlNodePtr& item : dataInterfaces->GetSequence())
        {
            if (!item || !item->IsMap())
            {
                error = "Each collection item must be a mapping.";
                return false;
            }
            for (const auto& property : item->GetMap())
            {
                auto it = propertiesWithEnumRef.find(property.first.c_str());
                if (it != propertiesWithEnumRef.end())
                {
                    if (!metaModel.ValidateEnumValue(property.first, it->second, property.second->GetScalar(), error))
                    {
                        return false;
                    }  
                }
            }
        }        
        return true;
    }

} // namespace acd

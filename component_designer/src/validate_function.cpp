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

        if (!InterfaceTypesSyntaxCheck(functionSpecification, error))
        {
            return false;
        }

        // ToDo
        // if (!metaModel.ValidateEnumValue("ASIL", "ASIL D", error))
        // {
        //     return false;
        // }

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

    bool ValidateFunction::InterfaceTypesSyntaxCheck(const YamlNodePtr& root, std::string& error)
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
        if (!SyntaxCheckForSequence(parameters, FunctionSpecification::kNamePathKey, error, FunctionSpecification::kParametersKey)) 
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

} // namespace acd

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

#include <algorithm>

#include "yaml_parser.h"

namespace acd 
{
    bool ValidateFunction::SyntaxCheckIsOK(std::string& content, std::string& error) 
    {
        YamlParser parser;
        const YamlNodePtr root = parser.ParseText(content, error);
        if (!root) 
        {
            return false;
        }
        const YamlNodePtr model = root->Find("functionSpecification");
        if (!model || !model->IsMap()) 
        {
            error = std::string("'") + std::string("functionSpecification") + "' root key not found";
            return false;
        }
        auto name = model->ScalarOf("name");
        auto version = model->ScalarOf("version");
     
        if (name.size() == 0 || version.size() == 0)
        {
            error = std::string("name or version not found");
            return false;
        }
        return true;
    }

} // namespace acd

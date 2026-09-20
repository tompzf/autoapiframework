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
 
#ifndef ACD_VALIDATE_FUNCTION_H
#define ACD_VALIDATE_FUNCTION_H

#include <map>
#include <string>
#include <vector>

#include "meta_model.h"
#include "yaml_node.h"

namespace acd 
{
    class ValidateFunction
    {
    public:
        bool SyntaxCheckIsOK(const MetaModel& metaModel, const std::string& content, std::string& error);
    
    private:
        bool HeaderSyntaxCheck(const YamlNodePtr& root, std::string& error);
        bool InterfaceTypesSyntaxCheck(const YamlNodePtr& root, std::string& error);
        bool SyntaxCheckForSequence(const YamlNodePtr& node, const std::string& key, std::string& error, const std::string& displayName);
    };
} // namespace acd

#endif // ACD_VALIDATE_FUNCTION_H

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
 
#ifndef ACD_FUNCTION_SPECIFICATION_H
#define ACD_FUNCTION_SPECIFICATION_H

#include <string>
#include <utility>
#include <vector>

#include "yaml_node.h"

namespace acd {

/// In-memory representation of a function specification (*.acs, YAML content).
///
/// The parsed YAML tree is kept as-is so that unknown keys survive a
/// load/save round trip and the file can be written in the original layout.
class FunctionSpecification {
public:
    static constexpr const char* kRootKey = "functionSpecification";
    static constexpr const char* kDataInterfacesKey = "dataInterfaces";
    static constexpr const char* kParametersKey = "parameters";
    static constexpr const char* kSchedulingKey = "scheduling";

    bool Load(const std::string& path, std::string& error);
    bool Save(const std::string& path, std::string& error) const;

    bool IsLoaded() const { return m_root != nullptr; }
    const std::string& GetSourcePath() const { return m_sourcePath; }

    std::string GetName() const;
    std::string GetVersion() const;
    std::string GetDescription() const;
    std::string GetMetaModelName() const;
    std::string GetMetaModelVersion() const;

    /// Header attributes (all scalar keys of `functionSpecification`) in file order.
    std::vector<std::pair<std::string, std::string>> GetAttributes() const;

    /// Entries of the signal, parameter or scheduling collection.
    std::vector<YamlNodePtr> GetCollection(const std::string& key) const;

private:
    const YamlNodePtr& Spec() const { return m_spec; }

    YamlNodePtr m_root;
    YamlNodePtr m_spec;
    std::string m_sourcePath;
};

} // namespace acd

#endif // ACD_FUNCTION_SPECIFICATION_H

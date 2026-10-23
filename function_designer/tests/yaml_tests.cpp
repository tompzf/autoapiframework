/********************************************************************************
 * Copyright (c) 2026 ZF Friedrichshafen AG
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
 *   Thomas Pfleiderer - add tests
 ********************************************************************************/

#include "meta_model.h"
#include "validate_function.h"
#include "yaml_node.h"
#include "yaml_parser.h"
#include "yaml_writer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    void Require(bool condition, const std::string& message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    afd::YamlNodePtr Parse(afd::YamlParser& parser, const std::string& text)
    {
        std::string error;
        afd::YamlNodePtr root = parser.ParseText(text, error);
        Require(root != nullptr, "parse failed: " + error);
        Require(error.empty(), "successful parse returned an error: " + error);
        return root;
    }

    void TestOrderedNodes()
    {
        afd::YamlNodePtr map = afd::YamlNode::MakeMap();
        map->Set("first", afd::YamlNode::MakeScalar("one"));
        map->Set("second", afd::YamlNode::MakeScalar("two"));
        map->Set("first", afd::YamlNode::MakeScalar("updated"));

        Require(map->GetMap().size() == 2, "replacing a map key added a duplicate");
        Require(map->GetMap()[0].first == "first", "replacing a key changed map order");
        Require(map->ScalarOf("first") == "updated", "replacing a key did not update its value");

        afd::YamlNodePtr sequence = afd::YamlNode::MakeSequence();
        sequence->Append(afd::YamlNode::MakeScalar("one"));
        Require(!sequence->RemoveAt(1), "removing an out-of-range sequence item succeeded");
        Require(sequence->RemoveAt(0), "removing a valid sequence item failed");
        Require(sequence->GetSequence().empty(), "removed sequence item is still present");
    }

    void TestScalarsAndInvalidInput()
    {
        afd::YamlParser parser;
        const afd::YamlNodePtr root = Parse(parser,
            "name: \"value # kept\" # comment\n"
            "quoted: 'it''s quoted'\n"
            "emptyMap: {}\n"
            "emptySequence: []\n"
            "description: >-\n"
            "  first line\n"
            "  second line\n"
            "literal: |-\n"
            "  first line\n"
            "  second line\n");

        Require(root->ScalarOf("name") == "value # kept", "quoted comment marker was stripped");
        Require(root->ScalarOf("quoted") == "it's quoted", "single-quoted scalar was not unescaped");
        Require(root->Find("emptyMap")->IsMap() && root->Find("emptyMap")->GetMap().empty(),
            "empty map was not parsed as a map");
        Require(root->Find("emptySequence")->IsSequence() && root->Find("emptySequence")->GetSequence().empty(),
            "empty sequence was not parsed as a sequence");
        Require(root->Find("description")->GetScalar() == "first line second line",
                "folded block scalar was not folded");
        Require(root->Find("literal")->GetScalar() == "first line\nsecond line",
                "literal block scalar lost its line break");

        std::string error;
        Require(parser.ParseText("name: first\n---\nname: second\n", error) == nullptr,
            "multiple YAML documents were accepted");
        Require(!error.empty(), "invalid input did not report an error");
    }

    void TestExampleRoundTrips()
    {
        const std::vector<std::string> examples = {
            "speed_hazard_detection.afs",
            "vehicle_speed_fusion_multirate.afs",
            "wheel_speed_plausibility_check.afs"
        };
        afd::YamlParser parser;
        afd::YamlWriter writer;

        for (const std::string& example : examples)
        {
            const std::string path = std::string(AFD_EXAMPLES_DIR) + "/" + example;
            std::string error;
            const afd::YamlNodePtr root = parser.ParseFile(path, error);
            Require(root != nullptr, example + " did not parse: " + error);

            const afd::YamlNodePtr specification = root->Find("functionSpecification");
            Require(specification && specification->IsMap(), example + " has no functionSpecification map");
            Require(!specification->ScalarOf("name").empty(), example + " has no function name");

            const afd::YamlNodePtr interfaces = specification->Find("dataInterfaces");
            Require(interfaces && interfaces->IsSequence() && !interfaces->GetSequence().empty(),
                    example + " has no dataInterfaces entries");
            Require(interfaces->GetSequence().front()->Find("name") != nullptr,
                    example + " first data interface has no name");

            const std::string serialized = writer.WriteText(root);
            const afd::YamlNodePtr reparsed = Parse(parser, serialized);
            const afd::YamlNodePtr reparsedSpecification = reparsed->Find("functionSpecification");
            Require(reparsedSpecification &&
                    reparsedSpecification->ScalarOf("name") == specification->ScalarOf("name"),
                    example + " changed its function name after writing and reparsing");
            const afd::YamlNodePtr reparsedInterfaces = reparsedSpecification->Find("dataInterfaces");
            Require(reparsedInterfaces && reparsedInterfaces->IsSequence() &&
                    reparsedInterfaces->GetSequence().size() == interfaces->GetSequence().size(),
                    example + " changed its data interface count after writing and reparsing");
        }
    }

    std::filesystem::path ResolveSpecificationPath(const std::string& inputPath, const char* executablePath)
    {
        std::error_code error;
        const std::filesystem::path requestedPath(inputPath);
        if (std::filesystem::exists(requestedPath, error) && !error)
        {
            return requestedPath;
        }

        error.clear();
        const std::filesystem::path executable = std::filesystem::absolute(executablePath, error);
        if (!error)
        {
            const std::filesystem::path besideExecutable = executable.parent_path() / requestedPath;
            if (std::filesystem::exists(besideExecutable, error) && !error)
            {
                return besideExecutable;
            }
        }

        error.clear();
        const std::filesystem::path examplePath = std::filesystem::path(AFD_EXAMPLES_DIR) / requestedPath;
        if (std::filesystem::exists(examplePath, error) && !error)
        {
            return examplePath;
        }
        return requestedPath;
    }

    int CheckSpecification(const std::string& inputPath, const char* executablePath)
    {
        const std::filesystem::path specificationPath = ResolveSpecificationPath(inputPath, executablePath);
        std::ifstream input(specificationPath, std::ios::binary);
        if (!input)
        {
            std::cerr << "Could not read function specification: " << inputPath << '\n';
            return 2;
        }
        std::ostringstream content;
        content << input.rdbuf();

        afd::MetaModel metaModel;
        std::string error;
        const std::string metaModelPath = std::string(AFD_EXAMPLES_DIR) + "/autoapiframework_metadata_V04.yaml";
        if (!metaModel.Load(metaModelPath, error))
        {
            std::cerr << "Could not load meta model: " << error << '\n';
            return 2;
        }

        afd::ValidateFunction validator;
        if (!validator.SyntaxCheckIsOK(metaModel, content.str(), error))
        {
            std::cerr << "Syntax error in " << inputPath << ":\n" << error << '\n';
            return 1;
        }

        std::cout << "Syntax OK: " << inputPath << '\n';
        return 0;
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        return CheckSpecification(argv[1], argv[0]);
    }
    if (argc != 1)
    {
        std::cerr << "Usage: function_designer_tests [file.afs]\n";
        return 2;
    }

    try
    {
        TestOrderedNodes();
        TestScalarsAndInvalidInput();
        TestExampleRoundTrips();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    std::cout << "All function designer tests passed.\n";
    return 0;
}
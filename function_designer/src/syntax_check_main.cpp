#include "meta_model.h"
#include "validate_function.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    constexpr const char* kMetaModelFileName = "autoapiframework_meta_model.yaml";

    std::filesystem::path FindMetaModel(const char* executablePath)
    {
        std::error_code error;
        const std::filesystem::path executable = std::filesystem::absolute(executablePath, error);
        if (!error)
        {
            const std::filesystem::path besideExecutable = executable.parent_path() / kMetaModelFileName;
            if (std::filesystem::exists(besideExecutable, error) && !error)
            {
                return besideExecutable;
            }
        }

        const std::filesystem::path currentDirectory = std::filesystem::current_path(error);
        if (!error)
        {
            const std::filesystem::path inCurrentDirectory = currentDirectory / kMetaModelFileName;
            if (std::filesystem::exists(inCurrentDirectory, error) && !error)
            {
                return inCurrentDirectory;
            }
        }
        return {};
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: AutoAPIFunctionDesignerSyntaxCheck <file.afs>\n";
        return 2;
    }

    const std::string specificationPath = argv[1];
    std::ifstream input(specificationPath, std::ios::binary);
    if (!input)
    {
        std::cout << "Could not read function specification: " << specificationPath << '\n';
        return 2;
    }
    std::ostringstream content;
    content << input.rdbuf();

    const std::filesystem::path metaModelPath = FindMetaModel(argv[0]);
    if (metaModelPath.empty())
    {
        std::cout << "Could not find " << kMetaModelFileName
                  << " beside the executable or in the current directory.\n";
        return 2;
    }

    afd::MetaModel metaModel;
    std::string error;
    if (!metaModel.Load(metaModelPath.string(), error))
    {
        std::cerr << "Could not load meta model: " << error << '\n';
        return 2;
    }

    afd::ValidateFunction validator;
    if (!validator.SyntaxCheckIsOK(metaModel, content.str(), error))
    {
        std::cerr << "Syntax error in " << specificationPath << ":\n" << error << '\n';
        return 1;
    }

    std::cout << "Syntax OK: " << specificationPath << '\n';
    return 0;
}
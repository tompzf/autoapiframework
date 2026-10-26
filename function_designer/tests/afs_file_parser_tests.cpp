#include "../src/afs_file_parser/afs_file_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    void Require(bool condition, const std::string& message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    std::string ReadFile(const std::string& path)
    {
        std::ifstream input(path);
        Require(static_cast<bool>(input), "could not open example: " + path);
        std::ostringstream content;
        content << input.rdbuf();
        return content.str();
    }

    void TestSampleExtraction()
    {
        afp::AfsFileParser parser;
        afp::FunctionSpecification functionSpec{};
        std::string error;
        const std::string path = std::string(AFS_PARSER_EXAMPLES_DIR) + "/wheel_speed_plausibility_check.afs";
        Require(parser.ParseAfsFile(ReadFile(path), functionSpec, error), "sample parse failed: " + error);
        Require(!functionSpec.signals.empty(), "signals were not extracted");
        Require(!functionSpec.parameters.empty(), "parameters were not extracted");
        Require(!functionSpec.scheduling.empty(), "scheduling entries were not extracted");

        bool foundLogicalSchedule = false;
        for (const afp::Schedule& schedule : functionSpec.scheduling)
        {
            if (schedule.functionName == "WheelSpeedPlausibilityCheck.Step")
            {
                foundLogicalSchedule = true;
                Require(schedule.supervisionLogical.predecessorRefs.size() == 1,
                        "logical predecessor was not extracted");
                Require(schedule.supervisionLogical.predecessorRefs.front() == "WheelSpeedPreProcessor.Step",
                        "logical predecessor value changed");
                Require(schedule.supervisionDeadline.timeout == 15,
                        "deadline timeout was not extracted");
            }
        }
        Require(foundLogicalSchedule, "expected schedule was not extracted");
    }

    void TestInvalidDocument()
    {
        afp::AfsFileParser parser;
        afp::FunctionSpecification functionSpec{};
        std::string error;
        Require(!parser.ParseAfsFile("notAnAfsDocument: true\n", functionSpec, error),
                "document without functionSpecification was accepted");
        Require(!error.empty(), "invalid document did not report an error");
    }
}

int main()
{
    try
    {
        TestSampleExtraction();
        TestInvalidDocument();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "ObjModel.hpp"
#include "CommandLineParser.hpp"

struct CommandLine
{
    sys::CharSeqArg filename;
    sys::BoolArg verbose;
    sys::BoolArg help;

    CommandLine(int argc, const char **argv);
};

CommandLine::CommandLine(int argc, const char **argv)
{
    sys::CommandLineParser clp;
    clp.parameter(filename).placeholder("FILE").description("The filename to load and parse.");
    clp.option(help).name("help").shortName("h").description("Display this help message.");
    clp.option(verbose).name("verbose").shortName("v").description("Display information by objects.");
    clp.validator([this](){
        if(help) return sys::OperationResult::succeeded();
        return sys::OperationResult::test(filename, "Missing OBJ filename!");
    });

    sys::OperationResult result = clp.parse(argc, argv);
    if (!result)
    {
        std::cerr << result.message() << std::endl << std::endl;
    }

    if (!result || help)
    {
        std::cerr << "Usage: " << argv[0] << " [OPTIONS] FILE" << std::endl;
        std::cerr << clp;
        std::exit(1);
    }
}

int main (int argc, const char **argv)
{
    CommandLine cmdLine(argc, argv);

    std::ifstream ifs(cmdLine.filename.value());
    if (!ifs.is_open())
    {
        std::clog << "Cannot open file " << cmdLine.filename.value() << std::endl;
        return 1;
    }

    vfm::ObjModel model;
    ifs >> model;
    ifs.close();

    std::clog << std::setw(12) << "Vertices: " << model.positions.size() << std::endl;
    std::clog << std::setw(12) << "Normals: " << model.normals.size() << std::endl;
    std::clog << std::setw(12) << "Textures: " << model.textures.size() << std::endl;
    std::clog << std::setw(12) << "Objects: " << model.objects.size() << std::endl;

    if (cmdLine.verbose.value())
    {
        for (vfm::Object &o : model.objects)
        {
            std::clog << "Object '" << o.name << "' has " << o.vertexIndices.size() << " vertices" << " and " << o.triangles.size()/3 << " triangles" << std::endl;
        }
    }

    return 0;
}


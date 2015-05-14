#include <iostream>
#include <iomanip>
#include <fstream>
#include "ObjModel.hpp"
#include "CommandLineParser.hpp"

struct Arguments
{
    sys::CharSeqArg filename;
    sys::BoolArg verbose;

    bool parse(int argc, const char **argv)
    {
        sys::CommandLineParser clp;
        clp.parameter(filename).placeholder("FILE").description("The filename to load and parse.");
        clp.option(verbose).name("verbose").shortName("v").description("Display information by objects.");

        sys::OperationResult result = clp.parse(argc, argv);
        if (!result)
        {
            std::cerr << result.message() << std::endl;
        }

        if (!filename)
        {
            std::cerr << "Missing file path!" << std::endl;
        }

        if (!result || !filename)
        {
            std::cerr << "Usage: " << argv[0] << " [OPTIONS] FILE" << std::endl;
            std::cerr << clp;
            return false;
        }
        return true;
    }
};

int main (int argc, const char **argv)
{
    Arguments args;

    if (!args.parse(argc, argv))
    {
        return 1;
    }

    std::ifstream ifs(args.filename.value());
    if (!ifs.is_open())
    {
        std::clog << "Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    vfm::ObjModel model;
    ifs >> model;
    ifs.close();

    std::clog << std::setw(12) << "Vertices: " << model.positions.size() << std::endl;
    std::clog << std::setw(12) << "Normals: " << model.normals.size() << std::endl;
    std::clog << std::setw(12) << "Textures: " << model.textures.size() << std::endl;
    std::clog << std::setw(12) << "Objects: " << model.objects.size() << std::endl;

    if (args.verbose.value())
    {
        for (vfm::Object &o : model.objects)
        {
            std::clog << "Object '" << o.name << "' has " << o.vertexIndices.size() << " vertices" << " and " << o.triangles.size()/3 << " triangles" << std::endl;
        }
    }

    return 0;
}


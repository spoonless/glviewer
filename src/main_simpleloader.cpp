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
        clp.argument(filename);
        clp.option(verbose).shortName("v");

        if (!clp.parse(argc, argv) || !filename)
        {
            std::clog << "Usage: " << argv[0] << " [-v] filename" << std::endl;
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


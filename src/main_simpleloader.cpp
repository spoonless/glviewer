#include <iostream>
#include <iomanip>
#include <fstream>
#include "ObjModel.hpp"

int main (int argc, char **argv)
{
    if (argc == 1)
    {
        std::clog << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.is_open())
    {
        std::clog << "Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    vfm::ObjModel model;
    ifs >> model;
    ifs.close();

    std::clog << std::setw(12) << "Vertices: " << model.vertices.size() << std::endl;
    std::clog << std::setw(12) << "Normals: " << model.normals.size() << std::endl;
    std::clog << std::setw(12) << "Textures: " << model.textures.size() << std::endl;
    std::clog << std::setw(12) << "Objects: " << model.objects.size() << std::endl;

    for (vfm::ObjectVector::iterator it = model.objects.begin(); it < model.objects.end(); ++it)
    {
        std::clog << "Object '" << it->name << "' has " << it->vertexIndices.size() << " vertices" << " and " << it->triangles.size()/3 << " triangles" << std::endl;
    }

    return 0;
}


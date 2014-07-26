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

    std::cout << std::setw(12) << "Vertices: " << model.vertices.size() << std::endl;
    std::cout << std::setw(12) << "Normals: " << model.normals.size() << std::endl;
    std::cout << std::setw(12) << "Textures: " << model.textures.size() << std::endl;
    std::cout << std::setw(12) << "Faces: " << model.faces.size() << std::endl;

    unsigned int triangleCount = 0;
    for (vfm::FaceVector::iterator it = model.faces.begin(); it != model.faces.end(); ++it)
    {
        triangleCount += it->vertexIndices.size() - 2;
    }
    std::cout << std::setw(12) << "Triangles: " << triangleCount << std::endl;

    return 0;
}


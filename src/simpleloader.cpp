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

    model::ObjModel model;
    ifs >> model;

    std::cout << std::setw(10) << "Vertices: " << model.vertices.size() << std::endl;
    std::cout << std::setw(10) << "Normals: " << model.normals.size() << std::endl;
    std::cout << std::setw(10) << "Faces: " << model.faces.size() << std::endl;

    return 0;
}


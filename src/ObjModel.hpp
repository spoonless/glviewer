#ifndef OBJMODEL_HPP
#define OBJMODEL_HPP

#include <vector>
#include <fstream>

#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

namespace vfm
{

typedef unsigned int index_t;

struct VertexIndex
{
    VertexIndex (index_t vertex = 0, index_t normal = 0);

    bool operator == (const VertexIndex &vi) const;

    index_t vertex;
//    index_t normal;
};

struct TriangleFace
{
    VertexIndex vertexIndices[3];
};

typedef std::vector<glm::vec4> Vec4Vector;
typedef std::vector<glm::vec3> Vec3Vector;
typedef std::vector<TriangleFace> TriangleFaceVector;

struct ObjModel
{
    Vec4Vector vertices;
    Vec3Vector normals;
    TriangleFaceVector faces;
};

std::istream & operator >> (std::istream &is, glm::vec3 &v);
std::istream & operator >> (std::istream &is, glm::vec4 &v);
std::istream & operator >> (std::istream &is, TriangleFace &face);
std::istream & operator >> (std::istream &is, ObjModel &vfm);

}

#endif // OBJMODEL_HPP

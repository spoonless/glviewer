#ifndef OBJMODEL_HPP
#define OBJMODEL_HPP

#include <vector>
#include <fstream>
#include <string>

#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

namespace vfm
{

typedef unsigned int index_t;
typedef std::vector<glm::vec4> Vec4Vector;
typedef std::vector<glm::vec3> Vec3Vector;

struct VertexIndex
{
    VertexIndex (index_t vertex = 0, index_t normal = 0, index_t texture = 0);

    bool operator == (const VertexIndex &vi) const;

    inline operator long*()
    {
        return &vertex;
    }

    inline operator const long*() const
    {
        return &vertex;
    }

    long vertex;
    long normal;
    long texture;
};

typedef std::vector<VertexIndex> VertexIndexVector;
typedef std::vector<index_t> IndexVector;

struct Object
{
    std::string name;
    VertexIndexVector vertexIndices;
    IndexVector triangles;
};

typedef std::vector<Object> ObjectVector;

struct ObjModel
{
    Vec4Vector vertices;
    Vec3Vector normals;
    Vec3Vector textures;
    ObjectVector objects;

    void computeNormals(bool normalized = false);
};

std::istream & operator >> (std::istream &is, glm::vec3 &v);
std::istream & operator >> (std::istream &is, glm::vec4 &v);
std::istream & operator >> (std::istream &is, VertexIndex &vi);
std::istream & operator >> (std::istream &is, VertexIndexVector &viv);
std::istream & operator >> (std::istream &is, ObjModel &vfm);

void computeNormals(ObjModel& model);

}

#endif // OBJMODEL_HPP

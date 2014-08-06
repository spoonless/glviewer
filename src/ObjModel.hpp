#ifndef OBJMODEL_HPP
#define OBJMODEL_HPP

#include <vector>
#include <fstream>
#include <string>
#include <map>

#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

namespace vfm
{

typedef unsigned int index_t;
typedef std::vector<glm::vec4> Vec4Vector;
typedef std::vector<glm::vec3> Vec3Vector;

struct Color
{
    Color() : ambiant(1.0f, 1.0f, 1.0f), diffuse(1.0f, 1.0f, 1.0f), specularCoeff(0.0f), dissolved(0.0f) {}

    glm::vec3 ambiant;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float specularCoeff;
    float dissolved;
};

struct Material
{
    Color color;
};

typedef std::map<std::string, Material> MaterialMap;

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

struct MaterialActivation
{
    MaterialActivation() : start(0), end(0) {}

    unsigned long start;
    unsigned long end;
    unsigned long materialLibrary;
    std::string name;
};

typedef std::vector<MaterialActivation> MaterialActivationVector;
typedef std::vector<VertexIndex> VertexIndexVector;
typedef std::vector<index_t> IndexVector;
typedef std::vector<std::string> MaterialLibraryVector;

struct Object
{
    std::string name;
    VertexIndexVector vertexIndices;
    IndexVector triangles;
    MaterialActivationVector materialActivations;
};

typedef std::vector<Object> ObjectVector;

struct ObjModel
{
    Vec4Vector vertices;
    Vec3Vector normals;
    Vec3Vector textures;
    ObjectVector objects;
    MaterialLibraryVector materialLibraries;

    void computeNormals(bool normalized = false);
};

std::istream & operator >> (std::istream &is, ObjModel &vfm);

std::istream & operator >> (std::istream &is, MaterialMap &materialMap);
}

#endif // OBJMODEL_HPP

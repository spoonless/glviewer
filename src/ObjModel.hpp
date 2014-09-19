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

typedef std::vector<glm::vec4> Vec4Vector;
typedef std::vector<glm::vec3> Vec3Vector;

struct Color
{
    Color() : ambient(1.0f, 1.0f, 1.0f), diffuse(1.0f, 1.0f, 1.0f), specularCoeff(0.0f), dissolved(0.0f) {}

    glm::vec3 ambient;
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
    VertexIndex (size_t vertex = 0, size_t normal = 0, size_t texture = 0);

    bool operator == (const VertexIndex &vi) const;

    inline operator size_t*()
    {
        return &vertex;
    }

    inline operator const size_t*() const
    {
        return &vertex;
    }

    size_t vertex;
    size_t normal;
    size_t texture;
};

struct MaterialId
{
    MaterialId() {}
    MaterialId(const std::string &library, const std::string &name) : library(library), name(name) {}

    std::string library;
    std::string name;

    bool operator == (const MaterialId &materialId) const
    {
        return this == &materialId || (this->library == materialId.library && this->name == materialId.name);
    }

    bool operator < (const MaterialId &materialId) const
    {
        return this->library < materialId.library && this->name < materialId.name;
    }
};

struct MaterialActivation
{
    MaterialActivation(size_t materialIndex, size_t start = 0u, size_t end = 0u) : materialIndex(materialIndex), start(start), end(end) {}

    size_t materialIndex;
    size_t start;
    size_t end;
};

typedef std::vector<MaterialId> MaterialIdVector;
typedef std::vector<MaterialActivation> MaterialActivationVector;
typedef std::vector<VertexIndex> VertexIndexVector;
typedef std::vector<size_t> IndexVector;

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
    MaterialIdVector materialIds;

    void computeNormals(bool normalized = false);
};

std::istream & operator >> (std::istream &is, ObjModel &vfm);

std::istream & operator >> (std::istream &is, MaterialMap &materialMap);
}

#endif // OBJMODEL_HPP

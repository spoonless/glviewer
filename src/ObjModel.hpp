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

typedef std::size_t MaterialIndex;
typedef std::vector<glm::vec4> Vec4Vector;
typedef std::vector<glm::vec3> Vec3Vector;

struct Color
{
    Color() : specularCoeff(.0f), dissolve(.0f) {}

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float specularCoeff;
    float dissolve;
};

struct TextureMap
{
    std::string ambient;
    std::string diffuse;
    std::string specular;
    std::string specularCoeff;
    std::string dissolve;
    std::string bump;
};

struct Material
{
    Color color;
    TextureMap map;
};

typedef std::map<std::string, Material> MaterialMap;

struct VertexIndex
{
    VertexIndex (std::size_t vertex = 0, std::size_t normal = 0, std::size_t texture = 0);

    bool operator == (const VertexIndex &vi) const;

    std::size_t vertex;
    std::size_t normal;
    std::size_t texture;
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
    MaterialActivation(MaterialIndex materialIndex, std::size_t start = 0u, std::size_t end = 0u) : materialIndex(materialIndex), start(start), end(end) {}

	MaterialIndex materialIndex;
    std::size_t start;
    std::size_t end;
};

typedef std::vector<MaterialId> MaterialIdVector;
typedef std::vector<MaterialActivation> MaterialActivationVector;
typedef std::vector<VertexIndex> VertexIndexVector;
typedef std::vector<std::size_t> IndexVector;

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
    Vec4Vector tangents;
    ObjectVector objects;
    MaterialIdVector materialIds;

    void computeNormals(bool normalized = false);
    void computeTangents();
};

std::istream & operator >> (std::istream &is, ObjModel &vfm);

std::istream & operator >> (std::istream &is, MaterialMap &materialMap);
}

#endif // OBJMODEL_HPP

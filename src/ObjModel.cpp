#include <memory>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include "glm/geometric.hpp"
#include "LineReader.hpp"
#include "ObjModel.hpp"

namespace
{

class VertexIndexIndexer
{
public:

    VertexIndexIndexer(vfm::Object *o) : _object{o}
    {
    }

    VertexIndexIndexer& operator = (vfm::Object *o)
    {
        if (_object != o)
        {
            _vertexIndexMap.clear();
            _object = o;
        }
        return *this;
    }

    std::size_t operator [](const vfm::VertexIndex &vi)
    {
        std::size_t mapSize = _vertexIndexMap.size();
        std::size_t *index = &_vertexIndexMap[vi];
        if (_vertexIndexMap.size() > mapSize)
        {
            *index = _object->vertexIndices.size();
            _object->vertexIndices.push_back(vi);
        }
        return *index;
    }

private:
    struct VertexIndexComparator {
        bool operator() (const vfm::VertexIndex &vi1, const vfm::VertexIndex &vi2) const
        {
          if (vi1.position == vi2.position)
          {
              if(vi1.normal == vi2.normal)
              {
                  return vi1.texture < vi2.texture;
              }
              return vi1.normal < vi2.normal;
          }
          return vi1.position < vi2.position;
        }
    };
    typedef std::map<vfm::VertexIndex, std::size_t, VertexIndexComparator> VertexIndexMap;

    vfm::Object *_object;
    VertexIndexMap _vertexIndexMap;
};

inline const char *nextToken(const char* l)
{
    const char *start = l;
    for(; std::isspace(*start) && *start != 0; ++start);
    return start;
}

inline char* read(const char *line, glm::vec4 &vec4)
{
    const char *token = line;
    vec4.x = vec4.y = vec4.z = 0;
    vec4.w = 1;
    char *endToken = 0;
    for (int i = 0; i < 4; ++i)
    {
        vec4[i] = static_cast<float>(strtod(token, &endToken));
        token = endToken;
        if(!std::isspace(*token))
        {
            break;
        }
    }
    return endToken;
}

inline char* read(const char *line, glm::vec3 &vec3)
{
    const char *token = line;
    vec3.x = vec3.y = vec3.z = 0;
    char *endToken = 0;
    for (int i = 0; i < 3; ++i)
    {
        vec3[i] = static_cast<float>(strtod(token, &endToken));
        token = endToken;
        if(!std::isspace(*token))
        {
            break;
        }
    }
    return endToken;
}

inline void createTriangles(const vfm::IndexVector &polygons, vfm::IndexVector &triangles)
{
    std::size_t nbIndices = polygons.size();
    if (nbIndices < 3)
    {
        return;
    }

    std::size_t descIndex = nbIndices;
    std::size_t ascIndex = 2;

    std::size_t previous = polygons[0];
    std::size_t current = polygons[1];
    std::size_t next = polygons[2];

    for(bool asc = false; ascIndex < descIndex; asc = !asc)
    {
        triangles.push_back(previous);
        triangles.push_back(current);
        triangles.push_back(next);

        if (asc)
        {
            current = next;
            next = polygons[++ascIndex];
        }
        else
        {
            current = previous;
            previous = polygons[--descIndex];
        }
    }
}

inline std::size_t readIndex(const char *token, char **endToken, std::size_t nbElements)
{
    long value = std::strtol(token, endToken, 10);
    std::size_t result = 0;
    if (value >= 0)
    {
        result = static_cast<std::size_t>(value);
    }
    else
    {
        value += nbElements + 1;
        if (value > 0)
        {
            result = static_cast<std::size_t>(value);
        }
    }
    return result;
}

void read (const char *line, const vfm::ObjModel &model, vfm::VertexIndexVector &face)
{
    const char *token = line;
    char* endToken = 0;
    vfm::VertexIndex vertexIndex;
    while(*token != 0 && std::isspace(*token))
    {
        vertexIndex.normal = vertexIndex.texture = 0;

        vertexIndex.position = readIndex(token, &endToken, model.positions.size());

        if (token == endToken)
        {
            break;
        }

        token = endToken;
        if (*endToken == '/')
        {
            vertexIndex.texture = readIndex(++token, &endToken, model.textures.size());
            token = endToken;
        }
        if (*endToken == '/')
        {
            vertexIndex.normal = readIndex(++token, &endToken, model.normals.size());
            token = endToken;
        }

        face.push_back(vertexIndex);
    }
}

vfm::MaterialIndex getMaterialIndex(vfm::ObjModel &model, const vfm::MaterialId &materialId)
{
    vfm::MaterialIdVector::iterator itFound = std::find(model.materialIds.begin(), model.materialIds.end(), materialId);
    if (itFound == model.materialIds.end())
    {
        model.materialIds.push_back(materialId);
        return model.materialIds.size() - 1;
    }
    else
    {
		return static_cast<vfm::MaterialIndex>(std::distance(model.materialIds.begin(), itFound));
    }
}

inline void endMaterialActivation(vfm::Object *object)
{
    if (!object->materialActivations.empty())
    {
        vfm::MaterialActivation &previousMaterialActivation = object->materialActivations.back();
        previousMaterialActivation.end = object->triangles.size();
        if (previousMaterialActivation.start >= previousMaterialActivation.end)
        {
            object->materialActivations.pop_back();
        }
    }
}

}

std::size_t vfm::ObjModel::nbTriangleVertices() const
{
    std::size_t nb = 0;
    for (const vfm::Object &o : this->objects)
    {
        nb += o.triangles.size();
    }
    return nb;
}

std::size_t vfm::ObjModel::nbVertexIndices() const
{
    std::size_t nb = 0;
    for (const vfm::Object &o : this->objects)
    {
        nb += o.vertexIndices.size();
    }
    return nb;
}

vfm::VertexIndex::VertexIndex (std::size_t vertex, std::size_t normal, std::size_t texture)
    : position(vertex), normal(normal), texture(texture)
{
}

bool vfm::VertexIndex::operator == (const VertexIndex &vi) const
{
    return this->position == vi.position && this->normal == vi.normal && this->texture == vi.texture;
}

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    glm::vec3 vec3;
    glm::vec4 vec4;
    vfm::IndexVector polygons;
    VertexIndexVector face(10);
	vfm::MaterialIndex currentMaterialIndex = 0;
    std::string mtllib;
    sys::LineReader lineReader(is);

    model.objects.push_back(Object());
    Object *object = &model.objects.back();
    VertexIndexIndexer vertexIndexIndexer(object);

    while(lineReader)
    {
        const char* line = lineReader.read();

        if(!std::strncmp("o ", line, 2))
        {
            if (!object->triangles.empty())
            {
                endMaterialActivation(object);
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer = object;

                if (currentMaterialIndex < model.materialIds.size())
                {
                    object->materialActivations.push_back(MaterialActivation(currentMaterialIndex));
                }
            }
            object->name = nextToken(line+2);
        }
        else if(!std::strncmp("v ", line, 2))
        {
            read(line+2, vec4);
            model.positions.push_back(vec4);
        }
        else if(!std::strncmp("vt ", line, 3))
        {
            read(line+3, vec3);
            // changing texture origin to lower left position
            vec3.y = 1 - vec3.y;
            model.textures.push_back(vec3);
        }
        else if(!std::strncmp("vn ", line, 3))
        {
            read(line+3, vec3);
            model.normals.push_back(vec3);
        }
        else if (!std::strncmp("f ", line, 2))
        {
            face.clear();
            read(line+1, model, face);

            polygons.clear();
            for(vfm::VertexIndexVector::iterator it = face.begin(); it < face.end(); ++it)
            {
                polygons.push_back(vertexIndexIndexer[*it]);
            }

            createTriangles(polygons, object->triangles);
        }
        else if (!std::strncmp("usemtl ", line, 7))
        {
            endMaterialActivation(object);
            currentMaterialIndex = getMaterialIndex(model, MaterialId(mtllib, line + 7));
            object->materialActivations.push_back(MaterialActivation(currentMaterialIndex, object->triangles.size()));
        }
        else if (!std::strncmp("mtllib ", line, 7))
        {
            mtllib = nextToken(line + 7);
        }
    }

    if (object->triangles.empty()){
        model.objects.pop_back();
    }
    else
    {
        endMaterialActivation(object);
    }

    return is;
}

std::istream & vfm::operator >> (std::istream &is, vfm::MaterialMap &materialMap)
{
    vfm::Material *material = 0;
    sys::LineReader lineReader{is};

    while(lineReader)
    {
        const char* line = lineReader.read();

        if (!std::strncmp(line, "newmtl ", 7))
        {
            material = &materialMap[std::string(nextToken(line+7))];
        }
        else if (material != 0)
        {
            if (!std::strncmp(line, "Ka ", 3))
            {
                read(line+3, material->color.ambient);
            }
            else if (!std::strncmp(line, "Kd ", 3))
            {
                read(line+3, material->color.diffuse);
            }
            else if (!std::strncmp(line, "Ks ", 3))
            {
                read(line+3, material->color.specular);
            }
            else if (!std::strncmp(line, "d ", 2))
            {
                material->color.dissolve = static_cast<float>(std::atof(line+2));
            }
            else if (!std::strncmp(line, "Tr ", 3))
            {
                material->color.dissolve = static_cast<float>(std::atof(line+3));
            }
            else if (!std::strncmp(line, "Ns ", 3))
            {
                material->color.specularCoeff = static_cast<float>(std::max(0.0, std::min(1000.0, std::atof(line+3))));
            }
            else if (!std::strncmp(line, "map_Ka ", 7))
            {
                material->map.ambient = nextToken(line + 7);
            }
            else if (!std::strncmp(line, "map_Kd ", 7))
            {
                material->map.diffuse = nextToken(line + 7);
            }
            else if (!std::strncmp(line, "map_Ks ", 7))
            {
                material->map.specular = nextToken(line + 7);
            }
            else if (!std::strncmp(line, "map_Ns ", 7))
            {
                material->map.specularCoeff = nextToken(line + 7);
            }
            else if (!std::strncmp(line, "map_d ", 6))
            {
                material->map.dissolve = nextToken(line + 6);
            }
            else if (!std::strncmp(line, "map_Tr ", 7))
            {
                material->map.dissolve = nextToken(line + 7);
            }
            else if (!std::strncmp(line, "bump ", 5))
            {
                material->map.bump = nextToken(line + 5);
            }
            else if (!std::strncmp(line, "map_Bump ", 9))
            {
                material->map.bump = nextToken(line + 9);
            }
        }
    }

    return is;
}


void vfm::ObjModel::computeNormals(bool normalized)
{
    this->normals.clear();
    this->normals.resize(this->positions.size());
    VertexIndex *vertexIndices[3];
    for(Object &o : this->objects)
    {
        for(IndexVector::iterator it = o.triangles.begin(); it != o.triangles.end(); ++it)
        {
            vertexIndices[0] = &o.vertexIndices[*it++];
            vertexIndices[1] = &o.vertexIndices[*it++];
            vertexIndices[2] = &o.vertexIndices[*it];
            glm::vec3 a(this->positions[vertexIndices[0]->position-1]);
            glm::vec3 b(this->positions[vertexIndices[1]->position-1]);
            glm::vec3 c(this->positions[vertexIndices[2]->position-1]);

            glm::vec3 normal = glm::normalize(glm::cross(b-a, c-a));
            for (int i = 0; i < 3; ++i)
            {
                std::size_t vertexIndex = vertexIndices[i]->position;
                this->normals[vertexIndex - 1] += normal;
                vertexIndices[i]->normal = vertexIndex;
            }
        }
    }

    if(normalized)
    {
        for(glm::vec3 &normal : this->normals)
        {
            normal = glm::normalize(normal);
        }
    }
}

void vfm::ObjModel::computeTangents()
{
    this->tangents.clear();
    if (this->normals.empty() || this->textures.empty())
    {
        return;
    }

    this->tangents.resize(this->normals.size());

    auto tan1 = std::unique_ptr<glm::vec3[]>(new glm::vec3[this->normals.size()]);
    auto tan2 = std::unique_ptr<glm::vec3[]>(new glm::vec3[this->normals.size()]);

    VertexIndex *vertexIndices[3];
    for(Object &o : this->objects)
    {
        for(auto it = o.triangles.begin(); it != o.triangles.end(); ++it)
        {
            vertexIndices[0] = &o.vertexIndices[*it++];
            vertexIndices[1] = &o.vertexIndices[*it++];
            vertexIndices[2] = &o.vertexIndices[*it];

            glm::vec4 a = this->positions[vertexIndices[1]->position-1] - this->positions[vertexIndices[0]->position-1];
            glm::vec4 b = this->positions[vertexIndices[2]->position-1] - this->positions[vertexIndices[0]->position-1];

            auto textureIndex0 = vertexIndices[0]->texture;
            auto textureIndex1 = vertexIndices[1]->texture;
            auto textureIndex2 = vertexIndices[2]->texture;

            if (textureIndex0 == 0 || textureIndex1 == 0 || textureIndex2 == 0)
            {
                continue;
            }

            glm::vec3 u = this->textures[textureIndex1 -1] - this->textures[textureIndex0 -1];
            glm::vec3 v = this->textures[textureIndex2 -1] - this->textures[textureIndex0 -1];

            float r = 1.0f / (u.s * v.t - v.s * u.t);

            glm::vec3 sdir{(v.t * a.x - u.t * b.x) * r, (v.t * a.y - u.t * b.y) * r, (v.t * a.z - u.t * b.z) * r};
            glm::vec3 tdir{(u.s * b.x - v.s * a.x) * r, (u.s * b.y - v.s * a.y) * r, (u.s * b.z - v.s * a.z) * r};

            for(std::size_t i = 0; i < 3; ++i)
            {
                auto normalIndex = vertexIndices[i]->normal;
                if (normalIndex == 0)
                {
                    continue;
                }
                tan1[normalIndex -1] += sdir;
                tan2[normalIndex -1] += tdir;
            }
        }
    }

    for(std::size_t i = 0; i < this->normals.size(); ++i)
    {
        glm::vec3 &n = this->normals[i];
        glm::vec3 &t = tan1[i];

        glm::vec3 xyzTangent = glm::normalize(t - n * glm::dot(n, t));
        glm::vec4 &tangent = this->tangents[i];

        tangent.x = xyzTangent.x;
        tangent.y = xyzTangent.y;
        tangent.z = xyzTangent.z;
        tangent.w = glm::dot(glm::cross(n, t), tan2[i]) < .0f ? -1.0f : 1.0f;
    }
}

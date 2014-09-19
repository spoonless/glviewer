#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <map>
#include "glm/geometric.hpp"
#include "ObjModel.hpp"

namespace
{

class VertexIndexIndexer
{
public:

    VertexIndexIndexer() : _object(0)
    {
    }

    VertexIndexIndexer& operator << (vfm::Object &o)
    {
        if (_object != &o)
        {
            _vertexIndexPtrMap.clear();
            _object = &o;
        }
        return *this;
    }

    vfm::index_t getIndex(const vfm::VertexIndex &vi)
    {
        size_t mapSize = _vertexIndexPtrMap.size();
        vfm::index_t *index = &_vertexIndexPtrMap[vi];
        if (_vertexIndexPtrMap.size() > mapSize)
        {
            *index = mapSize;
            _object->vertexIndices.push_back(vi);
        }
        return *index;
    }

private:
    struct VertexIndexPtrComparator {
        bool operator() (const vfm::VertexIndex &vi1, const vfm::VertexIndex &vi2) const
        {
          if (vi1.vertex == vi2.vertex)
          {
              if(vi1.normal == vi2.normal)
              {
                  return vi1.texture < vi2.texture;
              }
              return vi1.normal < vi2.normal;
          }
          return vi1.vertex < vi2.vertex;
        }
    };
    typedef std::map<vfm::VertexIndex, vfm::index_t, VertexIndexPtrComparator> VertexIndexMap;

    vfm::Object *_object;
    VertexIndexMap _vertexIndexPtrMap;
};

inline const char *nextToken(const char* l)
{
    const char *start = l;
    for(; std::isspace(*start) && *start != 0; ++start);
    return start;
}

class LineReader
{
    static const size_t BUFFER_CHUNK_SIZE;
public:
    LineReader(std::istream &is);

    ~LineReader();

    const char *read();

    inline operator bool() const
    {
        return _is.good();
    }

private:
    LineReader(const LineReader &);
    LineReader &operator = (const LineReader &);
    void copyReadLine();

    std::istream &_is;
    size_t _capacity;
    char *_line;
};

const size_t LineReader::BUFFER_CHUNK_SIZE = 256 * sizeof(char);

LineReader::LineReader(std::istream &is) : _is(is), _capacity(BUFFER_CHUNK_SIZE)
{
    _line = static_cast<char*>(std::malloc(_capacity * sizeof(char)));
    *_line = 0;
}

LineReader::~LineReader()
{
    std::free(_line);
}

const char *LineReader::read()
{
    copyReadLine();

    char *end = _line;
    for(; *end != 0; ++end)
    {
        if (*end == '#' || *end == '\r')
        {
            *end = 0;
            break;
        }
    }

    for(; end != _line && std::isspace(*(end-1)); --end);
    *end = 0;

    return nextToken(_line);
}

void LineReader::copyReadLine()
{
    *_line = 0;
    _is.getline(_line, _capacity);
    std::streamsize nbRead = 0;
    while((_is.rdstate() & std::istream::failbit) && _is.gcount() > 0 && !_is.eof())
    {
        nbRead += _is.gcount();
        _is.clear();
        _line = static_cast<char*>(std::realloc(_line, (_capacity + BUFFER_CHUNK_SIZE) * sizeof(char)));
        _is.getline(_line + nbRead, BUFFER_CHUNK_SIZE);
        _capacity += BUFFER_CHUNK_SIZE;
    }
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
    vfm::index_t nbIndices = polygons.size();
    if (nbIndices < 3)
    {
        return;
    }

    vfm::index_t descIndex = nbIndices;
    vfm::index_t ascIndex = 2;

    vfm::index_t previous = polygons[0];
    vfm::index_t current = polygons[1];
    vfm::index_t next = polygons[2];

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

void read (const char *line, const vfm::ObjModel &model, vfm::VertexIndexVector &face)
{
    const char *token = line;
    char* endToken = 0;
    vfm::VertexIndex vertexIndex;
    while(*token != 0 && std::isspace(*token))
    {
        vertexIndex.vertex = vertexIndex.normal = vertexIndex.texture = 0;

        vertexIndex.vertex = std::strtol(token, &endToken, 10);
        if(vertexIndex.vertex < 0)
        {
            vertexIndex.vertex = std::max(0ul, model.vertices.size() + vertexIndex.vertex + 1);
        }

        if (token == endToken)
        {
            break;
        }

        token = endToken;
        if (*endToken == '/')
        {
            vertexIndex.texture = std::strtol(++token, &endToken, 10);
            if(vertexIndex.texture < 0)
            {
                vertexIndex.texture = std::max(0ul, model.textures.size() + vertexIndex.texture + 1);
            }
            token = endToken;
        }
        if (*endToken == '/')
        {
            vertexIndex.normal = std::strtol(++token, &endToken, 10);
            if(vertexIndex.normal < 0)
            {
                vertexIndex.normal = std::max(0ul, model.normals.size() + vertexIndex.normal + 1);
            }
            token = endToken;
        }

        face.push_back(vertexIndex);
    }
}

unsigned int getMaterialIndex (vfm::ObjModel &model, const vfm::MaterialId &materialId)
{
    vfm::MaterialIdVector::iterator itFound = std::find(model.materialIds.begin(), model.materialIds.end(), materialId);
    if (itFound == model.materialIds.end())
    {
        model.materialIds.push_back(materialId);
        return model.materialIds.size() - 1;
    }
    else
    {
        return std::distance(model.materialIds.begin(), itFound);
    }
}

}

vfm::VertexIndex::VertexIndex (index_t vertex, index_t normal, index_t texture)
    : vertex(vertex), normal(normal), texture(texture)
{
}

bool vfm::VertexIndex::operator == (const VertexIndex &vi) const
{
    return this->vertex == vi.vertex && this->normal == vi.normal && this->texture == vi.texture;
}

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    glm::vec3 vec3;
    glm::vec4 vec4;
    vfm::IndexVector polygons;
    VertexIndexVector face(10);
    VertexIndexIndexer vertexIndexIndexer;
    MaterialActivation materialActivation;
    Object *object = 0;
    std::string mtllib;
    LineReader lineReader(is);

    while(lineReader)
    {
        const char* line = lineReader.read();

        if(!std::strncmp("o ", line, 2))
        {
            bool reuseMaterial = false;
            if (object != 0 && !object->materialActivations.empty())
            {
                object->materialActivations.back().end = object->triangles.size();
                reuseMaterial = true;
            }
            model.objects.push_back(Object());
            object = &model.objects.back();
            if (reuseMaterial)
            {
                materialActivation.start = object->triangles.size();
                object->materialActivations.push_back(materialActivation);
            }
            vertexIndexIndexer << *object;
            object->name = nextToken(line+2);
        }
        else if(!std::strncmp("v ", line, 2))
        {
            read(line+2, vec4);
            model.vertices.push_back(vec4);
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
            if (object == 0)
            {
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer << *object;
            }

            face.clear();
            read(line+1, model, face);

            polygons.clear();
            for(vfm::VertexIndexVector::iterator it = face.begin(); it < face.end(); ++it)
            {
                polygons.push_back(vertexIndexIndexer.getIndex(*it));
            }

            createTriangles(polygons, object->triangles);
        }
        else if (!std::strncmp("usemtl ", line, 7))
        {
            if (object == 0)
            {
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer << *object;
            }

            if (!object->materialActivations.empty())
            {
                MaterialActivation &previousMaterialActivation = object->materialActivations.back();
                previousMaterialActivation.end = object->triangles.size();
                if (previousMaterialActivation.start == previousMaterialActivation.end)
                {
                    object->materialActivations.pop_back();
                }
            }
            materialActivation.materialIndex = getMaterialIndex(model, MaterialId(mtllib, line + 7));
            materialActivation.start = object->triangles.size();
            object->materialActivations.push_back(materialActivation);
        }
        else if (!std::strncmp("mtllib ", line, 7))
        {
            mtllib = nextToken(line + 7);
        }
    }
    if (object != 0 && !object->materialActivations.empty())
    {
        object->materialActivations.back().end = object->triangles.size();
    }
    return is;
}

std::istream & vfm::operator >> (std::istream &is, vfm::MaterialMap &materialMap)
{
    vfm::Material *material = 0;
    LineReader lineReader(is);

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
                material->color.dissolved = static_cast<float>(std::atof(line+2));
            }
            else if (!std::strncmp(line, "Tr ", 3))
            {
                material->color.dissolved = static_cast<float>(std::atof(line+3));
            }
            else if (!std::strncmp(line, "Ns ", 3))
            {
                material->color.specularCoeff = static_cast<float>(std::max(0.0, std::min(1000.0, std::atof(line+3))));
            }
        }
    }

    return is;
}


void vfm::ObjModel::computeNormals(bool normalized)
{
    this->normals.clear();
    this->normals.resize(this->vertices.size());
    VertexIndex *vertexIndices[3];
    for(ObjectVector::iterator itObj = this->objects.begin(); itObj != this->objects.end(); ++itObj)
    {
        Object *o = &(*itObj);
        for(IndexVector::iterator it = o->triangles.begin(); it != o->triangles.end(); ++it)
        {
            vertexIndices[0] = &o->vertexIndices[*it++];
            vertexIndices[1] = &o->vertexIndices[*it++];
            vertexIndices[2] = &o->vertexIndices[*it];
            glm::vec3 a(this->vertices[vertexIndices[0]->vertex-1]);
            glm::vec3 b(this->vertices[vertexIndices[1]->vertex-1]);
            glm::vec3 c(this->vertices[vertexIndices[2]->vertex-1]);

            glm::vec3 normal = glm::normalize(glm::cross(b-a, c-a));
            for (int i = 0; i < 3; ++i)
            {
                index_t vertexIndex = vertexIndices[i]->vertex;
                this->normals[vertexIndex - 1] += normal;
                vertexIndices[i]->normal = vertexIndex;
            }
        }
    }

    if(normalized)
    {
        for(Vec3Vector::iterator itNormal = this->normals.begin(); itNormal != this->normals.end(); ++itNormal)
        {
            *itNormal = glm::normalize(*itNormal);
        }
    }
}

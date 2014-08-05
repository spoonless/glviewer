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

const size_t BUFFER_CHUNK_SIZE = 1024 * sizeof(char);

inline char* readline(std::istream &is, char *&line, size_t &lineCapacity)
{
    is.getline(line, lineCapacity);
    std::streamsize nbRead = 0;
    while((is.rdstate() & std::istream::failbit) && is.gcount() > 0 && !is.eof())
    {
        nbRead += is.gcount();
        is.clear();
        line = static_cast<char*>(std::realloc(line, lineCapacity + BUFFER_CHUNK_SIZE));
        is.getline(line + nbRead, BUFFER_CHUNK_SIZE);
        lineCapacity += BUFFER_CHUNK_SIZE;
    }

    for(char *end = line; *end != 0; ++end)
    {
        if (*end == '#' || *end == '\r')
        {
            *end = 0;
            break;
        }
    }

    return line;
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
    Object *object = 0;
    size_t lineCapacity = BUFFER_CHUNK_SIZE;
    char *line = static_cast<char*>(std::malloc(lineCapacity));

    while(is.good() && !is.eof())
    {
        char *token = readline(is, line, lineCapacity);

        if(!std::strncmp("o ", token, 2))
        {
            model.objects.push_back(Object());
            object = &model.objects.back();
            vertexIndexIndexer << *object;
            object->name = token+2;
        }
        else if(!std::strncmp("v ", token, 2))
        {
            token +=2;
            read(token, vec4);
            model.vertices.push_back(vec4);
        }
        else if(!std::strncmp("vt ", token, 3))
        {
            token +=3;
            read(token, vec3);
            model.textures.push_back(vec3);
        }
        else if(!std::strncmp("vn ", token, 3))
        {
            token +=3;
            read(token, vec3);
            model.normals.push_back(vec3);
        }
        else if (!std::strncmp("f ", token, 2))
        {
            if (object == 0)
            {
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer << *object;
            }

            face.clear();
            ++token;
            while(*token != 0 && std::isspace(*token))
            {
                long vertexIndice = 0;
                long textureIndice = 0;
                long normalIndice = 0;

                char *endToken = 0;
                vertexIndice = std::strtol(token, &endToken, 10);

                if (token == endToken)
                {
                    break;
                }

                token = endToken;
                if (*endToken == '/')
                {
                    textureIndice = std::strtol(++token, &endToken, 10);
                    token = endToken;
                }
                if (*endToken == '/')
                {
                    normalIndice = std::strtol(++token, &endToken, 10);
                    token = endToken;
                }

                face.push_back(VertexIndex(vertexIndice, normalIndice, textureIndice));
            }

            polygons.clear();
            for(vfm::VertexIndexVector::iterator it = face.begin(); it < face.end(); ++it)
            {
                if(it->vertex < 0)
                {
                    it->vertex = std::max(0ul, model.vertices.size() + it->vertex + 1);
                }
                if(it->normal < 0)
                {
                    it->normal = std::max(0ul, model.normals.size() + it->normal + 1);
                }
                if(it->texture < 0)
                {
                    it->texture = std::max(0ul, model.textures.size() + it->texture + 1);
                }
                polygons.push_back(vertexIndexIndexer.getIndex(*it));
            }

            createTriangles(polygons, object->triangles);
        }
    }

    std::free(line);

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

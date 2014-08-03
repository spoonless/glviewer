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

inline std::istream& eatline (std::istream& is)
{
    return is.ignore(std::numeric_limits<long>::max(), '\n');
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

std::istream & vfm::operator >> (std::istream &is, glm::vec3 &v)
{
    is >> v.x;
    if(is.good())
    {
        is >> v.y;
        if (is.fail())
        {
            v.y = 0;
            v.z = 0;
            is.clear();
        }
        else if(is.good())
        {
            is >> v.z;
            if (is.fail())
            {
                v.z = 0;
                is.clear();
            }
        }
    }
    return is;
}

std::istream & vfm::operator >> (std::istream &is, glm::vec4 &v)
{
    is >> v.x >> v.y >> v.z;
    if (is.good())
    {
        is >> v.w;
        // w is optional, no matter if the stream has not
        // succeeded in reading it
        if (is.fail())
        {
            v.w = 1;
            is.clear();
        }
    }
    return is;
}

std::istream & vfm::operator >> (std::istream &is, VertexIndex &vi)
{
    is >> vi.vertex;
    if (is.good() && is.peek() == '/')
    {
        is.ignore(1);
        if(is.good() && is.peek() != '/')
        {
            is >> vi.texture;
        }
        if(is.good() && is.peek() == '/')
        {
            is.ignore(1);
            is >> vi.normal;
        }
    }
    return is;
}

std::istream & vfm::operator >> (std::istream &is, VertexIndexVector &viv)
{
    bool eol = false;
    while(is.good() && !eol)
    {
        VertexIndex vi;
        is >> vi;
        viv.push_back(vi);
        while (is.good())
        {
            int c = is.peek();
            if (c == '#' || c == '\n')
            {
                is >> eatline;
                eol = true;
                break;
            }
            else if (std::isspace(c))
            {
                is.get();
            }
            else {
                break;
            }
        }
    }
    return is;
}

static void createTriangles(const vfm::IndexVector &polygons, vfm::IndexVector &triangles)
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

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    std::string token;
    glm::vec4 vec4;
    glm::vec3 vec3;
    VertexIndexIndexer vertexIndexIndexer;
    vfm::IndexVector polygons;
    VertexIndexVector face;
    Object *object = 0;

    while (is >> token)
    {
        if (! token.empty() && token[0] == '#')
        {
            // ignore comments
            is >> eatline;
        }
        else if (token == "o")
        {
            model.objects.push_back(Object());
            object = &model.objects.back();
            vertexIndexIndexer << *object;
            is >> object->name;
        }
        else if (token == "v")
        {
            // read new vertex
            is >> vec4;
            if(!is.fail()) model.vertices.push_back(vec4);
        }
        else if (token == "vt")
        {
            // read texture coordinates
            is >> vec3;
            if (!is.fail()) model.textures.push_back(vec3);
        }
        else if (token == "vn")
        {
            // read normals coordinates
            is >> vec3;
            if (!is.fail()) model.normals.push_back(vec3);
        }
        else if (token == "f")
        {
            if (object == 0)
            {
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer << *object;
            }

            face.clear();
            // read faces indices
            is >> face;

            if(!is.fail())
            {
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
        else
        {
            // ignore line
            is >> eatline;
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

#include <map>
#include <limits>
#include "ObjModel.hpp"

namespace vfm
{

struct Face
{
    VertexIndexVector vertexIndices;
};
std::istream & operator >> (std::istream &is, Face &face);

}

namespace
{

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

typedef std::map<vfm::VertexIndex, vfm::index_t, VertexIndexPtrComparator> VertexIndexPtrMap;

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
    if(is)
    {
        is >> v.y;
        if (is.fail())
        {
            v.y = 0;
            v.z = 0;
            is.clear();
        }
        else if(is)
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
    if (is)
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
    if (is && is.peek() == '/')
    {
        is.ignore(1);
        if(is && is.peek() != '/')
        {
            is >> vi.texture;
        }
        if(is && is.peek() == '/')
        {
            is.ignore(1);
            is >> vi.normal;
        }
    }
    return is;
}

std::istream & vfm::operator >> (std::istream &is, Face &face)
{
    bool eol = false;
    while(is && !eol)
    {
        VertexIndex vi;
        if (is >> vi)
        {
            face.vertexIndices.push_back(vi);
            while (is && !eol)
            {
                int c = is.peek();
                if (c == '#' || c == '\n')
                {
                    is >> eatline;
                    eol = true;
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
    }
    return is;
}

class VertexIndexIndexer
{
public:

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
    vfm::Object *_object;
    VertexIndexPtrMap _vertexIndexPtrMap;
};

static void createTriangles(const vfm::Face &face, VertexIndexIndexer &vertexIndexIndexer, vfm::IndexVector &triangles)
{
    vfm::index_t nbIndices = face.vertexIndices.size();
    if (nbIndices < 3)
    {
        return;
    }

    vfm::index_t descIndex = nbIndices;
    vfm::index_t ascIndex = 2;

    vfm::index_t previous = vertexIndexIndexer.getIndex(face.vertexIndices[0]);
    vfm::index_t current = vertexIndexIndexer.getIndex(face.vertexIndices[1]);
    vfm::index_t next = vertexIndexIndexer.getIndex(face.vertexIndices[2]);

    for(bool asc = false; ascIndex < descIndex; asc = !asc)
    {
        triangles.push_back(previous);
        triangles.push_back(current);
        triangles.push_back(next);

        if (asc)
        {
            current = next;
            next = vertexIndexIndexer.getIndex(face.vertexIndices[++ascIndex]);
        }
        else
        {
            current = previous % nbIndices;
            previous = vertexIndexIndexer.getIndex(face.vertexIndices[--descIndex]);
        }
    }
}

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    std::string token;
    glm::vec4 vec4;
    glm::vec3 vec3;
    Face face;
    Object *object = 0;
    VertexIndexIndexer vertexIndexIndexer;

    while (is && is >> token)
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
            if(!is.bad()) model.vertices.push_back(vec4);
        }
        else if (token == "vt")
        {
            // read texture coordinates
            is >> vec3;
            if (!is.bad()) model.textures.push_back(vec3);
        }
        else if (token == "vn")
        {
            // read normals coordinates
            is >> vec3;
            if (!is.bad()) model.normals.push_back(vec3);
        }
        else if (token == "f")
        {
            if (object == 0)
            {
                model.objects.push_back(Object());
                object = &model.objects.back();
                vertexIndexIndexer << *object;
            }

            face.vertexIndices.clear();
            // read faces indices
            is >> face;

            if(!is.bad())
            {
                createTriangles(face, vertexIndexIndexer, object->triangles);
            }
        }
        else if(is)
        {
            // ignore line
            is >> eatline;
        }
    }

    return is;
}

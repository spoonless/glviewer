#include <map>
#include <limits>
#include "ObjModel.hpp"

namespace
{

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

std::istream & vfm::operator >> (std::istream &is, VertexIndexVector &viv)
{
    bool eol = false;
    while(is && !eol)
    {
        VertexIndex vi;
        if (is >> vi)
        {
            viv.push_back(vi);
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

            face.clear();
            // read faces indices
            is >> face;

            if(!is.bad())
            {
                polygons.clear();
                for(vfm::VertexIndexVector::iterator it = face.begin(); it < face.end(); ++it)
                {
                    polygons.push_back(vertexIndexIndexer.getIndex(*it));
                }

                createTriangles(polygons, object->triangles);
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

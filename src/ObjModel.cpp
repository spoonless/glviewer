#include <set>
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
    bool operator() (const vfm::VertexIndex* vi1, const vfm::VertexIndex* vi2) const
    {
      if (vi1->vertex == vi2->vertex)
      {
          if(vi1->normal == vi2->normal)
          {
              return vi1->texture < vi2->texture;
          }
          return vi1->normal < vi2->normal;
      }
      return vi1->vertex < vi2->vertex;
    }
};

typedef std::set<vfm::VertexIndex const*, VertexIndexPtrComparator> VertexIndexPtrSet;

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

static void createTriangles(vfm::Object &object, const vfm::Face &face, VertexIndexPtrSet &vertexIndexPtrSet)
{
    for(vfm::VertexIndexVector::const_iterator it = face.vertexIndices.begin(); it != face.vertexIndices.end(); ++it)
    {
        std::pair<VertexIndexPtrSet::iterator, bool> p = vertexIndexPtrSet.insert(&(*it));
        vfm::VertexIndex **viptr = const_cast<vfm::VertexIndex**>(&(*p.first));
        if (p.second)
        {
            object.trianglesVector.push_back(object.vertexIndices.size());
            object.vertexIndices.push_back(*it);
            *viptr = &object.vertexIndices.back();
        }
        else
        {
            object.trianglesVector.push_back(std::distance(&object.vertexIndices[0], *viptr));
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
    VertexIndexPtrSet vertexIndexPtrSet;

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
            vertexIndexPtrSet.clear();
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
                vertexIndexPtrSet.clear();
            }

            face.vertexIndices.clear();
            // read faces indices
            is >> face;

            if(!is.bad())
            {
                createTriangles(*object, face, vertexIndexPtrSet);
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

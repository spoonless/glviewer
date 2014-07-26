#include <limits>
#include "ObjModel.hpp"

namespace
{

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

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    std::string token;
    glm::vec4 vec4;
    glm::vec3 vec3;
    Face face;
    while (is && is >> token)
    {
        if (! token.empty() && token[0] == '#')
        {
            // ignore comments
            is >> eatline;
        }
        else if (token == "v")
        {
            // read new vertex
            if (is >> vec4) model.vertices.push_back(vec4);
        }
        else if (token == "vt")
        {
            // read texture coordinates
            if (is >> vec3) model.textures.push_back(vec3);
        }
        else if (token == "vn")
        {
            // read normals coordinates
            if (is >> vec3) model.normals.push_back(vec3);
        }
        else if (token == "f")
        {
            face.vertexIndices.clear();
            is >> face;
            // read faces indices
            model.faces.push_back(face);
        }
        else if(is)
        {
            // ignore line
            is >> eatline;
        }
    }

    return is;
}

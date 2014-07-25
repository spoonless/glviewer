#include <limits>
#include "ObjModel.hpp"

namespace
{

inline std::istream& eatline (std::istream& is)
{
    return is.ignore(std::numeric_limits<long>::max(), '\n');
}

}

vfm::VertexIndex::VertexIndex (index_t vertex, index_t normal)
    : vertex(vertex) // , normal(normal)
{
}

bool vfm::VertexIndex::operator == (const VertexIndex &vi) const
{
    return this->vertex == vi.vertex; // && this->normal == vi.normal;
}

std::istream & vfm::operator >> (std::istream &is, glm::vec3 &v)
{
    return is >> v.x >> v.y >> v.z;
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

std::istream & vfm::operator >> (std::istream &is, TriangleFace &face)
{
    for (int i = 0; is && i < 3; ++i)
    {
        VertexIndex *vi = face.vertexIndices + i;
        is >> vi->vertex;
        if (is && is.peek() == '/')
        {
            is.ignore(1);
            index_t dummyIndex = 0;
            is >> dummyIndex;
            is.clear(is.rdstate() & ~std::istream::failbit);

            if(is && is.peek() == '/')
            {
                is.ignore(1);
                //is >> vi->normal;
                is >> dummyIndex;
            }
        }
    }

    return is;
}

std::istream & vfm::operator >> (std::istream &is, ObjModel &model)
{
    std::string token;
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
            model.vertices.push_back(glm::vec4());
            is >> model.vertices.back();
        }
        else if (token == "vt")
        {
            // read texture coordinates
            is >> eatline;
        }
        else if (token == "vn")
        {
            // read normals coordinates
            model.normals.push_back(glm::vec3());
            is >> model.normals.back();
        }
        else if (token == "f")
        {
            // read faces indices
            model.faces.push_back(TriangleFace());
            is >> model.faces.back();
        }
        else if(is)
        {
            // ignore line
            is >> eatline;
        }
    }

    return is;
}

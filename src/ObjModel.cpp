#include <limits>
#include "ObjModel.hpp"

model::Vertex4d::Vertex4d(coord_t x, coord_t y, coord_t z, coord_t w)
{
    coord.x = x;
    coord.y = y;
    coord.z = z;
    coord.w = w;
}

bool model::Vertex4d::operator == (const Vertex4d &v) const
{
    return this->coord.x == v.coord.x
            && this->coord.y == v.coord.y
            && this->coord.z == v.coord.z
            && this->coord.w == v.coord.w;
}

model::Vertex3d::Vertex3d(coord_t x, coord_t y, coord_t z)
{
    coord.x = x;
    coord.y = y;
    coord.z = z;
}

bool model::Vertex3d::operator == (const Vertex3d &v) const
{
    return this->coord.x == v.coord.x
            && this->coord.y == v.coord.y
            && this->coord.z == v.coord.z;
}

model::VertexIndex::VertexIndex (index_t vertex, index_t normal)
    : vertex(vertex), normal(normal)
{
}

bool model::VertexIndex::operator == (const VertexIndex &vi) const
{
    return this->vertex == vi.vertex && this->normal == vi.normal;
}

std::istream & model::operator >> (std::istream &is, Vertex3d &v)
{
    return is >> v.coord.x >> v.coord.y >> v.coord.z;
}

std::istream & model::operator >> (std::istream &is, Vertex4d &v)
{
    is >> v.coord.x >> v.coord.y >> v.coord.z;
    if (is)
    {
        is >> v.coord.w;
        // w is optional, no matter if the stream has not
        // succeeded in reading it
        if (is.fail())
        {
            v.coord.w = 1;
            is.clear();
        }
    }
    return is;
}

std::istream & model::operator >> (std::istream &is, TriangleFace &face)
{
    for (int i = 0; is && i < 3; ++i)
    {
        VertexIndex *vi = face.vertexIndices + i;
        is >> vi->vertex;
        if (is && is.peek() == '/')
        {
            is.ignore(1);
            index_t textureIndex = 0;
            is >> textureIndex;
            is.clear(is.rdstate() & ~std::istream::failbit);

            if(is && is.peek() == '/')
            {
                is.ignore(1);
                is >> vi->normal;
            }
        }
    }

    return is;
}

std::istream & model::operator >> (std::istream &is, ObjModel &model)
{
    std::string token;
    while (is && is >> token)
    {
        if (! token.empty() && token[0] == '#')
        {
            is.ignore(std::numeric_limits<long>::max(), '\n');
        }
        else if (token == "v")
        {
            // read new vertex
            model.vertices.push_back(Vertex4d());
            is >> model.vertices.back();
        }
        else if (token == "vt")
        {
            // read texture coodinates
            is.ignore(std::numeric_limits<long>::max(), '\n');
        }
        else if (token == "vn")
        {
            // read normals coordinates
            model.normals.push_back(Vertex3d());
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
            is.setstate(std::ios_base::failbit);
        }
    }

    return is;
}

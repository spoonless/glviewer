#include <gtest/gtest.h>

#include <istream>
#include <string>
#include <limits>

namespace wfm
{

typedef double coord_t;
typedef unsigned int index_t;

union Vertex4d
{
public:
    Vertex4d(coord_t x = 0, coord_t y = 0, coord_t z = 0, coord_t w = 1)
    {
        coord.x = x;
        coord.y = y;
        coord.z = z;
        coord.w = w;
    }

    bool operator == (const Vertex4d &v) const
    {
        return this->coord.x == v.coord.x
                && this->coord.y == v.coord.y
                && this->coord.z == v.coord.z
                && this->coord.w == v.coord.w;
    }

    struct
    {
        coord_t x;
        coord_t y;
        coord_t z;
        coord_t w;
    } coord;
    coord_t coords[4];
};

union Vertex3d
{
public:
    Vertex3d(coord_t x = 0, coord_t y = 0, coord_t z = 0)
    {
        coord.x = x;
        coord.y = y;
        coord.z = z;
    }

    bool operator == (const Vertex3d &v) const
    {
        return this->coord.x == v.coord.x
                && this->coord.y == v.coord.y
                && this->coord.z == v.coord.z;
    }

    struct
    {
        coord_t x;
        coord_t y;
        coord_t z;
    } coord;
    coord_t coords[3];
};

struct VertexIndex
{
    VertexIndex (index_t vertex = 0, index_t normal = 0) : vertex(vertex), normal(normal)
    {
    }

    bool operator == (const VertexIndex &vi) const
    {
        return this->vertex == vi.vertex && this->normal == vi.normal;
    }

    index_t vertex;
    index_t normal;
};

struct TriangleFace
{
    VertexIndex vertexIndices[3];
};

typedef std::vector<Vertex4d> Vertex4dVector;
typedef std::vector<Vertex3d> Vertex3dVector;
typedef std::vector<TriangleFace> TriangleFaceVector;

class WavefrontModel
{
public:
    Vertex4dVector vertices;
    Vertex3dVector normals;
    TriangleFaceVector faces;
};

std::istream & operator >> (std::istream &is, Vertex3d &v)
{
    return is >> v.coord.x >> v.coord.y >> v.coord.z;
}

std::istream & operator >> (std::istream &is, Vertex4d &v)
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

std::istream & operator >> (std::istream &is, TriangleFace &face)
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

std::istream & operator >> (std::istream &is, WavefrontModel &model)
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

}

#include <sstream>

TEST(wavefront, canLoadEmptyModel)
{
    wfm::WavefrontModel model;

    std::istringstream stream("");

    stream >> model;

    ASSERT_EQ(0u, model.vertices.size());
    ASSERT_EQ(0u, model.normals.size());
}

TEST(wavefront, canLoadVertices)
{
    wfm::WavefrontModel model;

    std::istringstream stream(
        "# only vertices\n"
        "v 0 0 0\n"
        "v 0.4 0.5 0.6\n"
        "v 1 1 1 0.5\n"
    );

    stream >> model;

    ASSERT_EQ(3u, model.vertices.size());
    ASSERT_EQ(wfm::Vertex4d(0,0,0,1), model.vertices[0]);
    ASSERT_EQ(wfm::Vertex4d(0.4,0.5,0.6,1), model.vertices[1]);
    ASSERT_EQ(wfm::Vertex4d(1,1,1,0.5), model.vertices[2]);
}

TEST(wavefront, canLoadNormals)
{
    wfm::WavefrontModel model;

    std::istringstream stream(
        "# only normals\n"
        "vn 0 0 0\n"
        "vn 0.4 0.5 0.6\n"
        "vn 1 1 1"
    );

    stream >> model;

    ASSERT_EQ(3u, model.normals.size());
    ASSERT_EQ(wfm::Vertex3d(0,0,0), model.normals[0]);
    ASSERT_EQ(wfm::Vertex3d(0.4,0.5,0.6), model.normals[1]);
    ASSERT_EQ(wfm::Vertex3d(1,1,1), model.normals[2]);
}

TEST(wavefront, canReadIndices)
{
    wfm::WavefrontModel model;

    std::istringstream stream(
        "# only face indices\n"
        "f 1 2 3\n"
        "f 4/40 5/50 6/60\n"
        "f 4/40/400 5/50/500 6/60/600\n"
        "f 7//700 8//800 9//900"
    );

    stream >> model;

    ASSERT_EQ(4u, model.faces.size());

    ASSERT_EQ(wfm::VertexIndex(1, 0), model.faces[0].vertexIndices[0]);
    ASSERT_EQ(wfm::VertexIndex(2, 0), model.faces[0].vertexIndices[1]);
    ASSERT_EQ(wfm::VertexIndex(3, 0), model.faces[0].vertexIndices[2]);

    ASSERT_EQ(wfm::VertexIndex(4, 0), model.faces[1].vertexIndices[0]);
    ASSERT_EQ(wfm::VertexIndex(5, 0), model.faces[1].vertexIndices[1]);
    ASSERT_EQ(wfm::VertexIndex(6, 0), model.faces[1].vertexIndices[2]);

    ASSERT_EQ(wfm::VertexIndex(4, 400), model.faces[2].vertexIndices[0]);
    ASSERT_EQ(wfm::VertexIndex(5, 500), model.faces[2].vertexIndices[1]);
    ASSERT_EQ(wfm::VertexIndex(6, 600), model.faces[2].vertexIndices[2]);

    ASSERT_EQ(wfm::VertexIndex(7, 700), model.faces[3].vertexIndices[0]);
    ASSERT_EQ(wfm::VertexIndex(8, 800), model.faces[3].vertexIndices[1]);
    ASSERT_EQ(wfm::VertexIndex(9, 900), model.faces[3].vertexIndices[2]);
}

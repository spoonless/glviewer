#ifndef OBJMODEL_HPP
#define OBJMODEL_HPP

#include <vector>
#include <fstream>

namespace model
{

typedef double coord_t;
typedef unsigned int index_t;

union Vertex4d
{
    Vertex4d(coord_t x = 0, coord_t y = 0, coord_t z = 0, coord_t w = 1);

    bool operator == (const Vertex4d &v) const;

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
    Vertex3d(coord_t x = 0, coord_t y = 0, coord_t z = 0);

    bool operator == (const Vertex3d &v) const;

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
    VertexIndex (index_t vertex = 0, index_t normal = 0);

    bool operator == (const VertexIndex &vi) const;

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

struct ObjModel
{
    Vertex4dVector vertices;
    Vertex3dVector normals;
    TriangleFaceVector faces;
};

std::istream & operator >> (std::istream &is, Vertex3d &v);
std::istream & operator >> (std::istream &is, Vertex4d &v);
std::istream & operator >> (std::istream &is, TriangleFace &face);
std::istream & operator >> (std::istream &is, ObjModel &model);

}


#endif // OBJMODEL_HPP

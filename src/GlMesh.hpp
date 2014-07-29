#ifndef GLMESH_H
#define GLMESH_H

#include <vector>
#include "gl.hpp"
#include "OperationResult.hpp"
#include "ObjModel.hpp"

namespace glv
{

typedef OperationResult GlMeshGeneration;

class GlMesh
{
public:
    GlMesh();
    ~GlMesh();

    GlMeshGeneration generate(const vfm::ObjModel &objModel);

    void render();


private:
    GlMesh(const GlMesh&);
    GlMesh& operator = (const GlMesh&);
    void clear();

    GLuint _vertexArray;
    std::vector<GLuint> _buffers;
    std::vector<GLsizei> _primitivesCount;
};

}

#endif

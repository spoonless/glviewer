#ifndef GLMESH_H
#define GLMESH_H

#include <vector>
#include "gl.hpp"
#include "OperationResult.hpp"
#include "ObjModel.hpp"
#include "UniformDeclaration.hpp"

namespace glv
{

typedef OperationResult GlMeshGeneration;
typedef OperationResult VertexAttributeDataDefinition;

class GlMesh
{
public:
    GlMesh();
    ~GlMesh();

    GlMeshGeneration generate(const vfm::ObjModel &objModel);
    VertexAttributeDataDefinition defineVertexAttributeData(const VertexAttributeDeclaration& vad);

    void render();


private:
    GlMesh(const GlMesh&);
    GlMesh& operator = (const GlMesh&);
    size_t getBufferIndex(const std::string &name);
    void generate(const vfm::ObjModel &objModel, unsigned int channel, std::vector<GLfloat> &buffer);
    void clear();

    GLuint _vertexArray;
    std::vector<GLuint> _buffers;
    std::vector<GLsizei> _primitivesCount;
};

}

#endif

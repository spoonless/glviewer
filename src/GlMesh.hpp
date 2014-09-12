#ifndef GLMESH_H
#define GLMESH_H

#include <vector>
#include "gl.hpp"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include "OperationResult.hpp"
#include "ObjModel.hpp"
#include "UniformDeclaration.hpp"

namespace glv
{

typedef OperationResult GlMeshGeneration;
typedef OperationResult VertexAttributeDataDefinition;

class BoundingBox
{
public:
    BoundingBox();

    glm::vec3 min;
    glm::vec3 max;

    void accept(float x, float y, float z);

    inline glm::vec3 center() const
    {
        return (min + max) * 0.5f;
    }
};

struct MaterialGroup
{
    static const unsigned int NO_MATERIAL_INDEX;

    MaterialGroup(unsigned int index, unsigned long size);

    unsigned int index;
    unsigned long size;
};

typedef std::vector<MaterialGroup> MaterialGroupVector;

class GlMesh
{
public:
    GlMesh();
    ~GlMesh();

    GlMeshGeneration generate(const vfm::ObjModel &objModel);
    VertexAttributeDataDefinition defineVertexAttributeData(const VertexAttributeDeclaration& vad);

    void render();

    inline const BoundingBox &getBoundingBox() const
    {
        return _boundingBox;
    }


private:
    GlMesh(const GlMesh&);
    GlMesh& operator = (const GlMesh&);
    size_t getBufferIndex(const std::string &name);
    void generate(const vfm::ObjModel &objModel, unsigned int channel, std::vector<GLfloat> &buffer);
    void clear();

    GLuint _vertexArray;
    BoundingBox _boundingBox;
    std::vector<GLuint> _buffers;
    std::vector<GLuint> _definedVertexAttributes;
    MaterialGroupVector _materialGroups;
};

}

#endif

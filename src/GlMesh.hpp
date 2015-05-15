#ifndef GLMESH_H
#define GLMESH_H

#include <vector>
#include "gl.hpp"
#include "glm/vec3.hpp"
#include "OperationResult.hpp"
#include "ObjModel.hpp"
#include "UniformDeclaration.hpp"

namespace ogl
{

using GlMeshGeneration = sys::OperationResult;

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

    inline glm::vec3 dimension() const
    {
        return max - min;
    }

    inline float width() const
    {
        return max.x - min.x;
    }

    inline float depth() const
    {
        return max.y - min.y;
    }

    inline float height() const
    {
        return max.z - min.z;
    }
};

typedef vfm::MaterialIndex MaterialIndex;

class MaterialHandler
{
public:
    static const MaterialIndex NO_MATERIAL_INDEX;

    virtual ~MaterialHandler() {}
    virtual void use(MaterialIndex index) = 0;
};

class GlMesh
{
public:
    GlMesh();
    ~GlMesh();
    GlMesh(const GlMesh&) = delete;
    GlMesh& operator = (const GlMesh&) = delete;

    GlMeshGeneration generate(vfm::ObjModel &objModel, const VertexAttributeDeclarationVector &vads);

    void render(MaterialHandler *handler = 0);

    inline const BoundingBox &getBoundingBox() const
    {
        return _boundingBox;
    }

private:
    struct MaterialGroup
    {
        MaterialGroup(MaterialIndex index, std::size_t size);

        MaterialIndex index;
        std::size_t size;
    };

    using MaterialGroupVector = std::vector<MaterialGroup>;

    void clear();
    void createMaterialGroups(const vfm::ObjModel &objModel);
    void createIndexBufferData(const vfm::ObjModel &objModel);

    GLuint _vertexArray;
    GLenum _indexFormat;
    std::vector<GLuint> _buffers;
    std::vector<GLuint> _definedVertexAttributes;
    MaterialGroupVector _materialGroups;
    BoundingBox _boundingBox;
};

}

#endif

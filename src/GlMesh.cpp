#include "GlError.hpp"
#include "Duration.hpp"
#include "GlMesh.hpp"

glv::GlMesh::GlMesh() : _vertexArray(0)
{
}

void glv::GlMesh::render()
{
    glBindVertexArray(_vertexArray);
    glEnableVertexAttribArray(0);

    GLint firstPrimitive = 0;
    for(std::vector<GLsizei>::iterator it = _primitivesCount.begin(); it != _primitivesCount.end(); ++it)
    {
        glDrawArrays(GL_TRIANGLES, firstPrimitive, *it);
        firstPrimitive += *it;
    }
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

glv::GlMesh::~GlMesh()
{
    clear();
}

void glv::GlMesh::clear()
{
    glBindVertexArray(0);
    if (_vertexArray > 0)
    {
        glDeleteVertexArrays(1, &_vertexArray);
        _vertexArray = 0;
    }
    if (!_buffers.empty())
    {
        glDeleteBuffers(_buffers.size(), &_buffers[0]);
    }
    _buffers.clear();
    _primitivesCount.clear();
}

glv::GlMeshGeneration glv::GlMesh::generate(const vfm::ObjModel &objModel)
{
    clear();
    GlError glError;
    Duration duration;

    glGenVertexArrays(1, &_vertexArray);
    if (glError)
    {
        return GlMeshGeneration(false, glError.toString("Error during vertex array generation"), duration.elapsed());
    }

    bool buffersAvailable [] = {! objModel.vertices.empty(), ! objModel.normals.empty(), ! objModel.textures.empty()};
    for(int i = 0; i < 3; ++i)
    {
        GLuint bufferId = 0;
        if (buffersAvailable[i])
        {
            glGenBuffers(1, &bufferId);
            if (glError)
            {
                return GlMeshGeneration(false, glError.toString("Error during buffer generation"), duration.elapsed());
            }
        }
        _buffers.push_back(bufferId);
    }

    unsigned int bufferElements = 0;
    for(vfm::ObjectVector::const_iterator it = objModel.objects.begin(); it != objModel.objects.end(); ++it)
    {
        bufferElements += it->triangles.size();
        _primitivesCount.push_back(it->triangles.size());
    }

    std::vector<GLfloat> tmpBuffer(bufferElements*3);
    int i = 0;
    for(vfm::ObjectVector::const_iterator it = objModel.objects.begin(); it != objModel.objects.end(); ++it)
    {
        const vfm::Object &o = *it;
        for(vfm::IndexVector::const_iterator idx = o.triangles.begin(); idx != o.triangles.end(); ++idx)
        {
            const vfm::VertexIndex &vertexIndex = o.vertexIndices[*idx];
            if (vertexIndex.vertex == 0)
            {
                tmpBuffer[i++] = 0; tmpBuffer[i++] = 0; tmpBuffer[i++] = 0;
            }
            else
            {
                const glm::vec4 &vec = objModel.vertices[vertexIndex.vertex -1];
                tmpBuffer[i++] = vec.x / vec.w; tmpBuffer[i++] = vec.y / vec.w; tmpBuffer[i++] = vec.z / vec.w;
            }
        }
    }

    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, tmpBuffer.size() * sizeof(GLfloat), &tmpBuffer[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    if (glError)
    {
        return GlMeshGeneration(false, glError.toString("Error while copying buffer data"), duration.elapsed());
    }

    return GlMeshGeneration(true, "", duration.elapsed());
}

size_t glv::GlMesh::getBufferIndex(const std::string &name)
{
    if (name == "position")
    {
        return 0;
    }
    else if (name == "normal")
    {
        return 1;
    }
    else if (name == "texture_coords")
    {
        return 2;
    }
    return _buffers.size();
}

glv::VertexAttributeDataDefinition glv::GlMesh::defineVertexAttributeData(const VertexAttributeDeclaration& vad)
{
    GlError glError;
    GLuint bufferIndex = getBufferIndex(vad.getName());
    if (bufferIndex >= _buffers.size() || _buffers[bufferIndex] == 0)
    {
        return VertexAttributeDataDefinition(false, std::string("No buffer available for vertex attribute '") + vad.getName() + "'");
    }
    if (vad.getSize() > 1)
    {
        return VertexAttributeDataDefinition(false, std::string("Vertex attribute '") + vad.getName() + "' cannot be an array");
    }

    GLint vertexAttributeSize = 0;
    GLsizei stride = 0;
    switch(vad.getType())
    {
    case GL_FLOAT_VEC3:
        vertexAttributeSize = 3;
        stride = 0;
        break;
    case GL_FLOAT_VEC2:
        vertexAttributeSize = 2;
        stride = 3*sizeof(GL_FLOAT);
        break;
    case GL_FLOAT:
        vertexAttributeSize = 1;
        stride = 3*sizeof(GL_FLOAT);
        break;
    default:
        return VertexAttributeDataDefinition(false, std::string("Invalid type for vertex attribute '") + vad.getName() + "': expected float, vec2 or vec3.");
    }

    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[bufferIndex]);
    glVertexAttribPointer(vad.getIndex(), vertexAttributeSize, GL_FLOAT, (bufferIndex == 1 ? GL_TRUE : GL_FALSE), stride, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    if (glError)
    {
        return VertexAttributeDataDefinition(false, glError.toString("defining vertex attribute"));
    }
    return VertexAttributeDataDefinition(true, "");
}


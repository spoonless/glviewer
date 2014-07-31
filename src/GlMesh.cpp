#include <limits>
#include "GlError.hpp"
#include "Duration.hpp"
#include "GlMesh.hpp"

#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT -std::numeric_limits<float>::max()

glv::BoundingBox::BoundingBox() : min(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT), max(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT)
{
}

void glv::BoundingBox::accept(float x, float y, float z)
{
    min.x = std::min(min.x, x);
    min.y = std::min(min.y, y);
    min.z = std::min(min.z, z);
    max.x = std::max(max.x, x);
    max.y = std::max(max.y, y);
    max.z = std::max(max.z, z);
}

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
    _boundingBox = BoundingBox();
    _buffers.clear();
    _buffers.resize(3);
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

    unsigned int bufferElements = 0;
    for(vfm::ObjectVector::const_iterator it = objModel.objects.begin(); it != objModel.objects.end(); ++it)
    {
        bufferElements += it->triangles.size();
        _primitivesCount.push_back(it->triangles.size());
    }

    std::vector<GLfloat> tmpBuffer(bufferElements*3);

    bool buffersAvailable [] = {! objModel.vertices.empty(), ! objModel.normals.empty(), ! objModel.textures.empty()};
    glBindVertexArray(_vertexArray);
    for(unsigned int i = 0; i < 3; ++i)
    {
        GLuint bufferId = 0;
        if (buffersAvailable[i])
        {
            glGenBuffers(1, &bufferId);
            if (glError)
            {
                glBindVertexArray(0);
                return GlMeshGeneration(false, glError.toString("Error during buffer generation"), duration.elapsed());
            }
            _buffers[i] = bufferId;
            generate(objModel, i, tmpBuffer);
            if (glError)
            {
                glBindVertexArray(0);
                return GlMeshGeneration(false, glError.toString("Error while copying buffer data"), duration.elapsed());
            }
        }
    }
    glBindVertexArray(0);
    return GlMeshGeneration(true, "", duration.elapsed());
}

void glv::GlMesh::generate(const vfm::ObjModel &objModel, unsigned int channel, std::vector<GLfloat> &buffer)
{
    if (!_buffers[channel])
    {
        return;
    }

    int i = 0;
    const glm::vec4 *vec4 = 0;
    const glm::vec3 *vec3 = 0;
    for(vfm::ObjectVector::const_iterator it = objModel.objects.begin(); it != objModel.objects.end(); ++it)
    {
        const vfm::Object &o = *it;
        for(vfm::IndexVector::const_iterator idx = o.triangles.begin(); idx != o.triangles.end(); ++idx)
        {
            const vfm::index_t *vertexIndex = o.vertexIndices[*idx];
            if (vertexIndex[channel] == 0)
            {
                buffer[i++] = 0; buffer[i++] = 0; buffer[i++] = 0;
            }
            else switch(channel)
            {
            case 0:
                vec4 = &objModel.vertices[vertexIndex[channel] -1];
                buffer[i++] = vec4->x / vec4->w; buffer[i++] = vec4->y / vec4->w; buffer[i++] = vec4->z / vec4->w;
                _boundingBox.accept(buffer[i-3], buffer[i-2], buffer[i-1]);
                break;
            case 1:
                vec3 = &objModel.normals[vertexIndex[channel] -1];
                buffer[i++] = vec3->x; buffer[i++] = vec3->y; buffer[i++] = vec3->z;
                break;
            case 2:
                vec3 = &objModel.textures[vertexIndex[channel] -1];
                buffer[i++] = vec3->x; buffer[i++] = vec3->y; buffer[i++] = vec3->z;
                break;
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, _buffers[channel]);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), &buffer[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    else if (name == "texture_coord")
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


#include <limits>
#include <algorithm>
#include "GlError.hpp"
#include "Duration.hpp"
#include "GlMesh.hpp"

#define MAX_UINT std::numeric_limits<unsigned int>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT -std::numeric_limits<float>::max()

const glv::MaterialIndex glv::MaterialHandler::NO_MATERIAL_INDEX = MAX_UINT;

glv::GlMesh::MaterialGroup::MaterialGroup(size_t index, size_t size) : index(index), size(size) {}

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

void glv::GlMesh::render(glv::MaterialHandler *handler)
{
    glBindVertexArray(_vertexArray);
    std::for_each(_definedVertexAttributes.begin(), _definedVertexAttributes.end(), glEnableVertexAttribArray);

    GLint firstPrimitive = 0;
    for(glv::GlMesh::MaterialGroupVector::iterator it = _materialGroups.begin(); it != _materialGroups.end(); ++it)
    {
        glv::GlMesh::MaterialGroup &materialGroup = *it;
        if (handler)
        {
            handler->use(materialGroup.index);
        }
        glDrawArrays(GL_TRIANGLES, firstPrimitive, static_cast<GLsizei>(materialGroup.size));
		firstPrimitive += static_cast<GLsizei>(materialGroup.size);
    }

    std::for_each(_definedVertexAttributes.begin(), _definedVertexAttributes.end(), glDisableVertexAttribArray);
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
		glDeleteBuffers(static_cast<GLsizei>(_buffers.size()), &_buffers[0]);
    }
    _boundingBox = BoundingBox();
    _buffers.clear();
    _buffers.resize(3);
    _definedVertexAttributes.clear();
    _materialGroups.clear();
}

glv::GlMeshGeneration glv::GlMesh::generate(const vfm::ObjModel &objModel)
{
    clear();
    GlError glError;
    sys::Duration duration;

    glGenVertexArrays(1, &_vertexArray);
    if (glError)
    {
        return GlMeshGeneration(false, glError.toString("Error during vertex array generation"), duration.elapsed());
    }

    size_t bufferElements = 0;
    for(vfm::ObjectVector::const_iterator it = objModel.objects.begin(); it != objModel.objects.end(); ++it)
    {
        const vfm::Object &o = *it;
        if(o.materialActivations.empty())
        {
            _materialGroups.push_back(MaterialGroup(MaterialHandler::NO_MATERIAL_INDEX, o.triangles.size()));
        }
        else
        {
            if(o.materialActivations[0].start > 0)
            {
                _materialGroups.push_back(MaterialGroup(MaterialHandler::NO_MATERIAL_INDEX, o.materialActivations[0].start));
            }
            for(vfm::MaterialActivationVector::const_iterator it = o.materialActivations.begin(); it != o.materialActivations.end(); ++it)
            {
                _materialGroups.push_back(MaterialGroup(it->materialIndex, it->end - it->start));
            }
        }
        bufferElements += o.triangles.size();
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
            const size_t *vertexIndex = o.vertexIndices[*idx];
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
    if (name == "vertexPosition")
    {
        return 0;
    }
    else if (name == "vertexNormal")
    {
        return 1;
    }
    else if (name == "vertexTextureCoord")
    {
        return 2;
    }
    return _buffers.size();
}

glv::VertexAttributeDataDefinition glv::GlMesh::defineVertexAttributeData(const VertexAttributeDeclaration& vad)
{
    GlError glError;
    size_t bufferIndex = getBufferIndex(vad.name());
    if (bufferIndex >= _buffers.size() || _buffers[bufferIndex] == 0)
    {
        return VertexAttributeDataDefinition(false, std::string("No buffer available for vertex attribute '") + vad.name() + "'");
    }
    if (vad.size() > 1)
    {
        return VertexAttributeDataDefinition(false, std::string("Vertex attribute '") + vad.name() + "' cannot be an array");
    }

    GLint vertexAttributeSize = 0;
    GLsizei stride = 0;
    switch(vad.type())
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
        return VertexAttributeDataDefinition(false, std::string("Invalid type for vertex attribute '") + vad.name() + "': expected float, vec2 or vec3.");
    }

    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[bufferIndex]);
    glVertexAttribPointer(vad.index(), vertexAttributeSize, GL_FLOAT, (bufferIndex == 1 ? GL_TRUE : GL_FALSE), stride, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    if (glError)
    {
        return VertexAttributeDataDefinition(false, glError.toString("defining vertex attribute"));
    }
    _definedVertexAttributes.push_back(vad.index());
    return VertexAttributeDataDefinition(true, "");
}


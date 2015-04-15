#include <limits>
#include <algorithm>
#include "GlError.hpp"
#include "Duration.hpp"
#include "GlMesh.hpp"

#define MAX_UINT std::numeric_limits<unsigned int>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT -std::numeric_limits<float>::max()

namespace
{
    enum VertexAttributeBuffer{VERTEX_POSITION, VERTEX_TEXTURE_COORD, VERTEX_NORMAL, VERTEX_TANGENT, NB_VERTEX_ATTRIBUTES};
}

const glv::MaterialIndex glv::MaterialHandler::NO_MATERIAL_INDEX = MAX_UINT;

glv::GlMesh::MaterialGroup::MaterialGroup(MaterialIndex index, size_t size) : index{index}, size{size} {}

glv::BoundingBox::BoundingBox() : min{MAX_FLOAT, MAX_FLOAT, MAX_FLOAT}, max{MIN_FLOAT, MIN_FLOAT, MIN_FLOAT}
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

glv::GlMesh::GlMesh() : _vertexArray{0}
{
}

void glv::GlMesh::render(glv::MaterialHandler *handler)
{
    glBindVertexArray(_vertexArray);
    std::for_each(_definedVertexAttributes.begin(), _definedVertexAttributes.end(), glEnableVertexAttribArray);

    GLint firstPrimitive = 0;
    for(glv::GlMesh::MaterialGroup &materialGroup : _materialGroups)
    {
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
    _boundingBox = BoundingBox{};
    _buffers.clear();
    _buffers.resize(NB_VERTEX_ATTRIBUTES);
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
        return GlMeshGeneration::failed(glError.toString("Error during vertex array generation"), duration.elapsed());
    }

    size_t bufferElements = 0;
    for(const vfm::Object &o : objModel.objects)
    {
        if(o.materialActivations.empty())
        {
            _materialGroups.push_back(MaterialGroup{MaterialHandler::NO_MATERIAL_INDEX, o.triangles.size()});
        }
        else
        {
            if(o.materialActivations[0].start > 0)
            {
                _materialGroups.push_back(MaterialGroup{MaterialHandler::NO_MATERIAL_INDEX, o.materialActivations[0].start});
            }
            for(vfm::MaterialActivationVector::const_iterator it = o.materialActivations.begin(); it != o.materialActivations.end(); ++it)
            {
                _materialGroups.push_back(MaterialGroup{it->materialIndex, it->end - it->start});
            }
        }
        bufferElements += o.triangles.size();
    }

    std::vector<GLfloat> tmpBuffer(bufferElements * NB_VERTEX_ATTRIBUTES);

    bool buffersAvailable [NB_VERTEX_ATTRIBUTES] = {false};
    buffersAvailable[VERTEX_POSITION] = ! objModel.vertices.empty();
    buffersAvailable[VERTEX_NORMAL] = ! objModel.normals.empty();
    buffersAvailable[VERTEX_TEXTURE_COORD] = ! objModel.textures.empty();
    buffersAvailable[VERTEX_TANGENT] = ! objModel.tangents.empty();

    glBindVertexArray(_vertexArray);
    for(size_t i = 0; i < NB_VERTEX_ATTRIBUTES; ++i)
    {
        GLuint bufferId = 0;
        if (buffersAvailable[i])
        {
            glGenBuffers(1, &bufferId);
            if (glError)
            {
                glBindVertexArray(0);
                return GlMeshGeneration::failed(glError.toString("Error during buffer generation"), duration.elapsed());
            }
            _buffers[i] = bufferId;
            generate(objModel, i, tmpBuffer);
            if (glError)
            {
                glBindVertexArray(0);
                return GlMeshGeneration::failed(glError.toString("Error while copying buffer data"), duration.elapsed());
            }
        }
    }
    glBindVertexArray(0);
    return GlMeshGeneration::succeeded(duration.elapsed());
}

void glv::GlMesh::generate(const vfm::ObjModel &objModel, size_t channel, std::vector<GLfloat> &buffer)
{
    if (!_buffers[channel])
    {
        return;
    }

    int i = 0;
    const glm::vec4 *vec4 = 0;
    const glm::vec3 *vec3 = 0;
    for(const vfm::Object &o : objModel.objects)
    {
        for(size_t index : o.triangles)
        {
            const vfm::VertexIndex &vertexIndex = o.vertexIndices[index];
            GLfloat x, y, z, w = 0;
            switch(channel)
            {
            case VERTEX_POSITION:
                if (vertexIndex.vertex > 0)
                {
                    vec4 = &objModel.vertices[vertexIndex.vertex -1];
                    x = vec4->x / vec4->w;
                    y = vec4->y / vec4->w;
                    z = vec4->z / vec4->w;
                    _boundingBox.accept(x, y, z);

                }
                break;
            case VERTEX_NORMAL:
                if (vertexIndex.normal > 0)
                {
                    vec3 = &objModel.normals[vertexIndex.normal -1];
                    x = vec3->x;
                    y = vec3->y;
                    z = vec3->z;
                }
                break;
            case VERTEX_TANGENT:
                if (vertexIndex.normal > 0)
                {
                    vec4 = &objModel.tangents[vertexIndex.normal -1];
                    x = vec4->x;
                    y = vec4->y;
                    z = vec4->z;
                    w = vec4->w;
                }
                break;
            case VERTEX_TEXTURE_COORD:
                if (vertexIndex.texture > 0)
                {
                    vec3 = &objModel.textures[vertexIndex.texture -1];
                    x = vec3->x;
                    y = vec3->y;
                    z = vec3->z;
                }
                break;
            }
            buffer[i++] = x;
            buffer[i++] = y;
            buffer[i++] = z;
            buffer[i++] = w;
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
        return VERTEX_POSITION;
    }
    else if (name == "vertexNormal")
    {
        return VERTEX_NORMAL;
    }
    else if (name == "vertexTextureCoord")
    {
        return VERTEX_TEXTURE_COORD;
    }
    else if (name == "vertexTangent")
    {
        return VERTEX_TANGENT;
    }
    return NB_VERTEX_ATTRIBUTES;
}

glv::VertexAttributeDataDefinition glv::GlMesh::defineVertexAttributeData(const VertexAttributeDeclaration& vad)
{
    GlError glError;
    size_t bufferIndex = getBufferIndex(vad.name());
    if (bufferIndex >= _buffers.size() || _buffers[bufferIndex] == 0)
    {
        return VertexAttributeDataDefinition::failed(std::string("No buffer available for vertex attribute '") + vad.name() + "'");
    }
    if (vad.size() > 1)
    {
        return VertexAttributeDataDefinition::failed(std::string("Vertex attribute '") + vad.name() + "' cannot be an array");
    }

    GLint vertexAttributeSize = 0;
    GLsizei stride = 0;
    switch(vad.type())
    {
    case GL_FLOAT_VEC4:
        vertexAttributeSize = 4;
        stride = 0;
        break;
    case GL_FLOAT_VEC3:
        vertexAttributeSize = 3;
        stride = 4*sizeof(GL_FLOAT);
        break;
    case GL_FLOAT_VEC2:
        vertexAttributeSize = 2;
        stride = 4*sizeof(GL_FLOAT);
        break;
    case GL_FLOAT:
        vertexAttributeSize = 1;
        stride = 4*sizeof(GL_FLOAT);
        break;
    default:
        return VertexAttributeDataDefinition::failed(std::string("Invalid type for vertex attribute '") + vad.name() + "': expected float, vec2, vec3 or vec4.");
    }

    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[bufferIndex]);
    glVertexAttribPointer(vad.index(), vertexAttributeSize, GL_FLOAT, (bufferIndex == VERTEX_NORMAL ? GL_TRUE : GL_FALSE), stride, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    if (glError)
    {
        return VertexAttributeDataDefinition::failed(glError.toString("defining vertex attribute"));
    }
    _definedVertexAttributes.push_back(vad.index());
    return VertexAttributeDataDefinition::succeeded();
}


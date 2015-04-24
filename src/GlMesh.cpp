#include <memory>
#include <limits>
#include <algorithm>
#include "GlError.hpp"
#include "Duration.hpp"
#include "GlMesh.hpp"

namespace
{
    const auto MAX_UINT = std::numeric_limits<unsigned int>::max();
    const auto MAX_GL_UNSIGNED_BYTE = std::numeric_limits<GLubyte>::max();
    const auto MAX_GL_UNSIGNED_SHORT = std::numeric_limits<GLushort>::max();
    const auto MAX_FLOAT = std::numeric_limits<float>::max();
    const auto MIN_FLOAT = -std::numeric_limits<float>::max();

    enum VertexAttributeBuffer{VERTEX_POSITION, VERTEX_TEXTURE_COORD, VERTEX_NORMAL, VERTEX_TANGENT, NB_VERTEX_ATTRIBUTES};

    void copy(GLfloat *dest, const glm::vec4 &source, std::size_t destWitdh, bool homogeneous = false)
    {
        switch(destWitdh)
        {
        case 4:
            dest[3] = source.w;
        case 3:
            dest[2] = source.z;
        case 2:
            dest[1] = source.y;
        case 1:
            dest[0] = source.x;
        }

        if (homogeneous && destWitdh < 4 && source.w != .0f)
        {
            switch(destWitdh)
            {
            case 3:
                dest[2] /= source.w;
            case 2:
                dest[1] /= source.w;
            case 1:
                dest[0] /= source.w;
            }
        }
    }

    void copy(GLfloat *dest, const glm::vec3 &source, std::size_t destWitdh)
    {
        switch(destWitdh)
        {
        case 4:
            dest[3] = 1.0f;
        case 3:
            dest[2] = source.z;
        case 2:
            dest[1] = source.y;
        case 1:
            dest[0] = source.x;
        }
    }

    VertexAttributeBuffer toVertexAttributeBuffer(const gl::VertexAttributeDeclaration &vad)
    {
        if (vad.name() == "vertexPosition")
        {
            return VERTEX_POSITION;
        }
        else if (vad.name() == "vertexNormal")
        {
            return VERTEX_NORMAL;
        }
        else if (vad.name() == "vertexTextureCoord")
        {
            return VERTEX_TEXTURE_COORD;
        }
        else if (vad.name() == "vertexTangent")
        {
            return VERTEX_TANGENT;
        }
        return NB_VERTEX_ATTRIBUTES;
    }

    struct VertexAttributeBufferDesc
    {
        GLuint index;
        VertexAttributeBuffer type;
        std::size_t offset;
        std::size_t size;
    };

    using VertexAttributeBufferDescVector = std::vector<VertexAttributeBufferDesc>;

    inline std::size_t computeVertexAttributesStructureSize(const VertexAttributeBufferDescVector &v)
    {
        return v.empty() ? 0 : v.back().offset + v.back().size;
    }

    VertexAttributeBufferDescVector createVertexAttributeBufferDescVector(const gl::VertexAttributeDeclarationVector &vads)
    {
        std::vector<VertexAttributeBufferDesc> vertexAttributeBufferDescVector;
        std::size_t vertexAttributePointerOffset = 0;
        VertexAttributeBufferDesc vabd;
        for(const gl::VertexAttributeDeclaration &vad : vads)
        {
            vabd.index = vad.index();
            vabd.size = vad.sizeOf() / sizeof(GLfloat);
            vabd.type = toVertexAttributeBuffer(vad);
            vabd.offset = vertexAttributePointerOffset;
            vertexAttributeBufferDescVector.push_back(vabd);

            vertexAttributePointerOffset += vabd.size;
        }
        return vertexAttributeBufferDescVector;
    }

    gl::GlMeshGeneration checkAndComputeVertexAttributes(const gl::VertexAttributeDeclarationVector &vads, vfm::ObjModel &objModel)
    {
        bool noBufferAvailable = false;
        int nbVertexAttributes = 0;
        for(const gl::VertexAttributeDeclaration &vad : vads)
        {
            switch(toVertexAttributeBuffer(vad))
            {
            case VERTEX_POSITION:
                noBufferAvailable = objModel.positions.empty();
                break;
            case VERTEX_NORMAL:
                if (objModel.normals.empty())
                {
                    objModel.computeNormals();
                    noBufferAvailable = objModel.normals.empty();
                }
                break;
            case VERTEX_TANGENT:
                if (objModel.tangents.empty())
                {
                    objModel.computeTangents();
                    noBufferAvailable = objModel.tangents.empty();
                }
                break;
            case VERTEX_TEXTURE_COORD:
                noBufferAvailable = objModel.textures.empty();
                break;

            default:
                noBufferAvailable = true;
            }

            if (noBufferAvailable)
            {
                return gl::GlMeshGeneration::failed(std::string("No buffer available for vertex attribute '") + vad.name() + "'!");
            }

            if (vad.type() != GL_FLOAT_VEC2 && vad.type() != GL_FLOAT_VEC3 && vad.type() != GL_FLOAT_VEC4)
            {
                return gl::GlMeshGeneration::failed(std::string("Invalid type for vertex attribute '") + vad.name() + "': expected float, vec2, vec3 or vec4.");
            }
            ++nbVertexAttributes;
        }

        if (nbVertexAttributes == 0)
        {
            return gl::GlMeshGeneration::failed("No vertex attributes found!");
        }
        return gl::GlMeshGeneration::succeeded();
    }

    void fillBuffer(GLfloat *tmpBuffer, gl::BoundingBox &boundingBox, const vfm::ObjModel &objModel, const VertexAttributeBufferDescVector &vertexAttributeBufferDescVector)
    {
        int tmpBufferOffset = 0;
        std::size_t vertexAttributesStructureSize = computeVertexAttributesStructureSize(vertexAttributeBufferDescVector);

        for(const vfm::Object &o : objModel.objects)
        {
            for(const vfm::VertexIndex &vertexIndex : o.vertexIndices)
            {
                for (const VertexAttributeBufferDesc &vabd : vertexAttributeBufferDescVector)
                {
                    switch (vabd.type) {
                    case VERTEX_POSITION:
                        if (vertexIndex.position != 0)
                        {
                            const glm::vec4 & position = objModel.positions[vertexIndex.position-1];
                            copy(&tmpBuffer[tmpBufferOffset + vabd.offset], position, vabd.size, true);
                            boundingBox.accept(position.x / position.w, position.y / position.w, position.z / position.w);
                        }
                        break;
                    case VERTEX_NORMAL:
                        if (vertexIndex.normal != 0)
                        {
                            copy(&tmpBuffer[tmpBufferOffset + vabd.offset], objModel.normals[vertexIndex.normal-1], vabd.size);
                        }
                        break;
                    case VERTEX_TANGENT:
                        if (vertexIndex.normal != 0)
                        {
                            copy(&tmpBuffer[tmpBufferOffset + vabd.offset], objModel.tangents[vertexIndex.normal-1], vabd.size);
                        }
                        break;
                    case VERTEX_TEXTURE_COORD:
                        if (vertexIndex.texture != 0)
                        {
                            copy(&tmpBuffer[tmpBufferOffset + vabd.offset], objModel.textures[vertexIndex.texture-1], vabd.size);
                        }
                        break;
                    default:
                        break;
                    }
                }
                tmpBufferOffset += vertexAttributesStructureSize;
            }
        }
    }

    template<typename T>
    void createPackedIndexBufferData(const vfm::ObjModel &objModel, std::size_t nbIndices)
    {
        auto indices = std::unique_ptr<T[]>(new T[nbIndices]);
        std::size_t i = 0;
        std::size_t startIndex = 0;
        for (const vfm::Object &o : objModel.objects)
        {
            for(std::size_t index : o.triangles)
            {
                indices[i++] = static_cast<T>(startIndex + index);
            }
            startIndex += o.vertexIndices.size();
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbIndices * sizeof(T), &indices[0], GL_STATIC_DRAW);
    }

}

const gl::MaterialIndex gl::MaterialHandler::NO_MATERIAL_INDEX = MAX_UINT;

gl::GlMesh::MaterialGroup::MaterialGroup(MaterialIndex index, std::size_t size) : index{index}, size{size} {}

gl::BoundingBox::BoundingBox() : min{MAX_FLOAT, MAX_FLOAT, MAX_FLOAT}, max{MIN_FLOAT, MIN_FLOAT, MIN_FLOAT}
{
}

void gl::BoundingBox::accept(float x, float y, float z)
{
    min.x = std::min(min.x, x);
    min.y = std::min(min.y, y);
    min.z = std::min(min.z, z);
    max.x = std::max(max.x, x);
    max.y = std::max(max.y, y);
    max.z = std::max(max.z, z);
}

gl::GlMesh::GlMesh() : _vertexArray{0}, _indexFormat{GL_UNSIGNED_SHORT}
{
}

void gl::GlMesh::render(gl::MaterialHandler *handler)
{
    glBindVertexArray(_vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[0]);
    std::for_each(_definedVertexAttributes.begin(), _definedVertexAttributes.end(), glEnableVertexAttribArray);

    std::size_t firstPrimitive = 0;
    std::size_t sizeofIndex = gl::glSizeof(_indexFormat);
    for(gl::GlMesh::MaterialGroup &materialGroup : _materialGroups)
    {
        if (handler)
        {
            handler->use(materialGroup.index);
        }
        glDrawElements(GL_TRIANGLES, materialGroup.size, _indexFormat, (void*)(firstPrimitive * sizeofIndex));
        firstPrimitive += materialGroup.size;
    }

    std::for_each(_definedVertexAttributes.begin(), _definedVertexAttributes.end(), glDisableVertexAttribArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

gl::GlMesh::~GlMesh()
{
    clear();
}

void gl::GlMesh::clear()
{
    glBindVertexArray(0);
    if (_vertexArray > 0)
    {
        glDeleteVertexArrays(1, &_vertexArray);
        _vertexArray = 0;
    }
    if (! _buffers.empty())
    {
        glDeleteBuffers(_buffers.size(), &_buffers[0]);
        _buffers.clear();
    }
    _boundingBox = {};
    _definedVertexAttributes.clear();
    _materialGroups.clear();
}

void gl::GlMesh::createMaterialGroups(const vfm::ObjModel &objModel)
{
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
            for(const vfm::MaterialActivation &ma : o.materialActivations)
            {
                _materialGroups.push_back(MaterialGroup{ma.materialIndex, ma.end - ma.start});
            }
        }
    }
}

void gl::GlMesh::createIndexBufferData(const vfm::ObjModel &objModel)
{
    std::size_t nbIndices = objModel.nbTriangleVertices();

    if(nbIndices < MAX_GL_UNSIGNED_BYTE)
    {
        this->_indexFormat = GL_UNSIGNED_BYTE;
        createPackedIndexBufferData<GLubyte>(objModel, nbIndices);
    }
    else if(nbIndices < MAX_GL_UNSIGNED_SHORT)
    {
        this->_indexFormat = GL_UNSIGNED_SHORT;
        createPackedIndexBufferData<GLushort>(objModel, nbIndices);
    }
    else
    {
        this->_indexFormat = GL_UNSIGNED_INT;
        createPackedIndexBufferData<GLuint>(objModel, nbIndices);
    }
}

gl::GlMeshGeneration gl::GlMesh::generate(vfm::ObjModel &objModel, const gl::VertexAttributeDeclarationVector &vads)
{
    clear();
    sys::Duration duration;

    GlMeshGeneration result = checkAndComputeVertexAttributes(vads, objModel);
    if (!result) {
        return result;
    }

    createMaterialGroups(objModel);

    VertexAttributeBufferDescVector vertexAttributeBufferDescVector = createVertexAttributeBufferDescVector(vads);
    std::size_t vertexAttributesStructureSize = computeVertexAttributesStructureSize(vertexAttributeBufferDescVector);

    std::vector<GLfloat> vertexAttributeBuffer(objModel.nbVertexIndices() * vertexAttributesStructureSize);
    fillBuffer(vertexAttributeBuffer.data(), _boundingBox, objModel, vertexAttributeBufferDescVector);

    GlError glError;

    glGenVertexArrays(1, &_vertexArray);
    if (glError)
    {
        return GlMeshGeneration::failed(glError.toString("Error during vertex array generation"), duration.elapsed());
    }

    glBindVertexArray(_vertexArray);
    _buffers.resize(2);
    glGenBuffers(_buffers.size(), &_buffers[0]);
    if (glError)
    {
        glBindVertexArray(0);
        return GlMeshGeneration::failed(glError.toString("Error during buffers generation"), duration.elapsed());
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[0]);
    createIndexBufferData(objModel);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);

    for (VertexAttributeBufferDesc vabd : vertexAttributeBufferDescVector)
    {
        glVertexAttribPointer(vabd.index, vabd.size, GL_FLOAT, (vabd.type == VERTEX_NORMAL ? GL_TRUE : GL_FALSE), (vertexAttributesStructureSize * sizeof(GL_FLOAT)), (void*)(vabd.offset  * sizeof(GL_FLOAT)));
        _definedVertexAttributes.push_back(vabd.index);
    }
    glBufferData(GL_ARRAY_BUFFER, vertexAttributeBuffer.size() * sizeof(GLfloat), vertexAttributeBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (glError)
    {
        return GlMeshGeneration::failed(glError.toString("defining vertex attribute"));
    }

    return GlMeshGeneration::succeeded(duration.elapsed());
}

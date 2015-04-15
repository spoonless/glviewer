#define GLM_FORCE_RADIANS
#include <functional>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include "config.h"
#include "gl.hpp"
#include "SOIL.h"
#include "glm/gtx/transform.hpp"
#include "GlWindowContext.hpp"
#include "log.hpp"
#include "Path.hpp"
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlMesh.hpp"
#include "Camera.hpp"

const double PI = std::atan(1.0)*4;

bool endsWith (const char *base, const char *str) {
    int blen = std::strlen(base);
    int slen = std::strlen(str);
    return (blen >= slen) && (0 == std::strcmp(base + blen - slen, str));
}

const char defaultMesh[] =
        "v -1 -1  0\n"
        "v  1 -1  0\n"
        "v  1  1  0\n"
        "v -1  1  0\n"
        "f  1 2 3 4";

const char defaultVertexShader[] =
        GLSL_VERSION_HEADER
        "in vec2 vertexPosition;\n"
        "out vec2 surfacePosition;\n"
        "void main(){\n"
        "  gl_Position = vec4(vertexPosition, 0, 1);\n"
        "  surfacePosition = vertexPosition;\n"
        "}\n";

const char defaultFragmentShader[] =
        GLSL_VERSION_HEADER
        "in vec2 surfacePosition;\n"
        "out vec4 color;\n"

        "uniform float time;\n"

        "const float color_intensity = .5;\n"
        "const float Pi = 3.14159;\n"

        "void main()\n"
        "{\n"
          "vec2 p=(1.32*surfacePosition);\n"
          "for(int i=1;i<5;i++)\n"
          "{\n"
            "vec2 newp=p;\n"
            "newp.x+=.912/float(i)*sin(float(i)*Pi*p.y+time*0.15)+0.91;\n"
            "newp.y+=.913/float(i)*cos(float(i)*Pi*p.x+time*-0.14)-0.91;\n"
            "p=newp;\n"
          "}\n"
          "vec3 col=vec3((sin(p.x+p.y)*.91+.1)*color_intensity);\n"
          "color=vec4(col, 1.0);\n"
        "}\n";

class TextureLoader
{
public:

    TextureLoader() {}

    ~TextureLoader()
    {
        std::vector<GLuint> texturesId;
        std::transform(_textureIdMap.begin(), _textureIdMap.end(), std::back_inserter(texturesId), getTextureId);
        glDeleteTextures(texturesId.size(), &texturesId[0]);
    }

    GLuint load(const sys::Path &basepath, const std::string &filename)
    {
        GLuint textureId = 0u;
        if (! filename.empty())
        {
            sys::Path filepath(basepath, filename.c_str());
            if (_textureIdMap.find(filename) != _textureIdMap.end()){
                return _textureIdMap[filename];
            }
            sys::Duration duration;
            textureId = SOIL_load_OGL_texture(filepath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS);
            if (textureId)
            {
                _textureIdMap[filename] = textureId;
                message(filepath, duration.elapsed());
            }
            else
            {
                warn(filepath, SOIL_last_result());
            }
        }
        return textureId;
    }

private:
    void warn(const char *filename, const char *message)
    {
        LOG(WARNING) << "error while loading '" << filename << "': " << message;
    }

    void message(const char *filename, unsigned long duration)
    {
        LOG(INFO) << "loading '" << filename << "' in " << duration << "ms.";
    }

    typedef std::map<const std::string, GLuint> TextureIdMap;
    static GLuint getTextureId(TextureIdMap::value_type &t) { return t.second ;}

    TextureLoader(const TextureLoader&);
    TextureLoader & operator = (const TextureLoader&);
    TextureIdMap _textureIdMap;
};

struct LoadedTexture{

    LoadedTexture(): ambient(0), diffuse(0), specular(0), specularCoeff(0), dissolve(0), bump(0)
    {
    }

    GLuint ambient;
    GLuint diffuse;
    GLuint specular;
    GLuint specularCoeff;
    GLuint dissolve;
    GLuint bump;
};

struct LoadedMaterial
{
    vfm::Color color;
    LoadedTexture texture;
};

class MaterialHandler : public glv::MaterialHandler
{
public:

    void loadUniforms(const glv::ShaderProgram &shaderProgram)
    {
        _uniformColor.load(shaderProgram);
        _uniformTexture.load(shaderProgram);
    }

    void loadMaterials(TextureLoader &textureLoader, const char *objFilename,  const vfm::ObjModel &model)
    {
        std::map<std::string, vfm::MaterialMap> materialMaps;
        sys::Path objFilepath(objFilename);
        sys::Path currentPath = objFilepath.dirpath();
        std::string defaultMaterialLibrary = std::string(objFilepath.withoutExtension()) + ".mtl";

        _materials.clear();

        for(vfm::MaterialIdVector::const_iterator it = model.materialIds.begin(); it != model.materialIds.end(); ++it)
        {
            const std::string *libraryName = &it->library;
            if (libraryName->empty())
            {
                libraryName = &defaultMaterialLibrary;
            }
            if (materialMaps.find(*libraryName) == materialMaps.end())
            {
                sys::Path mtlfile(currentPath, libraryName->c_str());
                sys::Duration loadfileDuration;
                std::ifstream isMat(mtlfile);

                if(isMat >> materialMaps[*libraryName])
                {
                    message(mtlfile, loadfileDuration.elapsed());
                }
                else
                {
                    warn(mtlfile, "Cannot read file (maybe the path is wrong)!");
                    materialMaps.erase(*libraryName);
                }
            }

            if (materialMaps.find(*libraryName) != materialMaps.end())
            {
                vfm::MaterialMap &materialMap = materialMaps[*libraryName];
                _materials.push_back(LoadedMaterial());
                if (materialMap.find(it->name) != materialMap.end())
                {
                    vfm::Material &material = materialMap[it->name];
                    LoadedMaterial &loadedMaterial = _materials.back();

                    loadedMaterial.color = material.color;

                    if (_uniformTexture.hasTexture())
                    {
                        sys::Path basePath = sys::Path(currentPath, libraryName->c_str()).dirpath();

                        loadedMaterial.texture.ambient = textureLoader.load(basePath, material.map.ambient);
                        loadedMaterial.texture.diffuse = textureLoader.load(basePath, material.map.diffuse);
                        loadedMaterial.texture.specular = textureLoader.load(basePath, material.map.specular);
                        loadedMaterial.texture.specularCoeff = textureLoader.load(basePath, material.map.specularCoeff);
                        loadedMaterial.texture.dissolve = textureLoader.load(basePath, material.map.dissolve);
                        loadedMaterial.texture.bump = textureLoader.load(basePath, material.map.bump);
                    }
                }
            }
        }
    }

    virtual void use(glv::MaterialIndex index)
    {
        if (index != NO_MATERIAL_INDEX && index < _materials.size())
        {
            LoadedMaterial &material = _materials[index];
            _uniformColor.use(material.color);
            _uniformTexture.use(material.texture);
        }
    }

private:
    void warn(const char *filename, const char *message)
    {
        LOG(WARNING) << "error while loading '" << filename << "': " << message;
    }

    void message(const char *filename, unsigned long duration)
    {
        LOG(INFO) << "loading '" << filename << "' in " << duration << "ms.";
    }

    class
    {
    public:

        void load(const glv::ShaderProgram &shaderProgram)
        {
            _ambiantSampler = shaderProgram.getActiveUniform("material.ambient");
            _diffuseSampler = shaderProgram.getActiveUniform("material.diffuse");
            _specularSampler = shaderProgram.getActiveUniform("material.specular");
            _specularCoeffSampler = shaderProgram.getActiveUniform("material.specularShininess");
        }

        void use(const vfm::Color &color)
        {
            if (_ambiantSampler)
            {
                *_ambiantSampler = color.ambient;
            }
            if (_diffuseSampler)
            {
                *_diffuseSampler = color.diffuse;
            }
            if (_specularSampler)
            {
                *_specularSampler = color.specular;
            }
            if (_specularCoeffSampler)
            {
                *_specularCoeffSampler = color.specularCoeff;
            }
        }

    private:
        glv::UniformDeclaration _ambiantSampler;
        glv::UniformDeclaration _diffuseSampler;
        glv::UniformDeclaration _specularSampler;
        glv::UniformDeclaration _specularCoeffSampler;
    } _uniformColor;

    class
    {
    public:

        void load(const glv::ShaderProgram &shaderProgram)
        {
            _ambiantSampler = shaderProgram.getActiveUniform("materialTexture.ambient.sampler");
            _diffuseSampler = shaderProgram.getActiveUniform("materialTexture.diffuse.sampler");
            _specularSampler = shaderProgram.getActiveUniform("materialTexture.specular.sampler");
            _specularCoeffSampler = shaderProgram.getActiveUniform("materialTexture.specularShininess.sampler");
            _dissolveSampler = shaderProgram.getActiveUniform("materialTexture.dissolve.sampler");
            _bumpSampler = shaderProgram.getActiveUniform("materialTexture.bump.sampler");

            _ambiantEnable = shaderProgram.getActiveUniform("materialTexture.ambient.enable");
            _diffuseEnable = shaderProgram.getActiveUniform("materialTexture.diffuse.enable");
            _specularEnable = shaderProgram.getActiveUniform("materialTexture.specular.enable");
            _specularCoeffEnable = shaderProgram.getActiveUniform("materialTexture.specularShininess.enable");
            _dissolveEnable = shaderProgram.getActiveUniform("materialTexture.dissolve.enable");
            _bumpEnable = shaderProgram.getActiveUniform("materialTexture.bump.enable");
        }

        inline bool hasTexture() const
        {
            return _ambiantSampler || _diffuseSampler || _specularSampler || _specularCoeffSampler || _dissolveSampler || _bumpSampler;
        }

        void use(const LoadedTexture &loadedTexture)
        {
            if (_ambiantSampler && loadedTexture.ambient)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.ambient);
                *_ambiantSampler = 0;
                *_ambiantEnable = true;
            }
            else
            {
                *_ambiantEnable = false;
            }

            if (_diffuseSampler && loadedTexture.diffuse)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.diffuse);
                *_diffuseSampler = 1;
                *_diffuseEnable = true;
            }
            else
            {
                *_diffuseEnable = false;
            }

            if (_specularSampler && loadedTexture.specular)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.specular);
                *_specularSampler = 2;
                *_specularEnable = true;
            }
            else
            {
                *_specularEnable = false;
            }

            if (_specularCoeffSampler && loadedTexture.specularCoeff)
            {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.specularCoeff);
                *_specularCoeffSampler = 3;
                *_specularCoeffEnable = true;
            }
            else
            {
                *_specularCoeffEnable = false;
            }

            if (_dissolveSampler && loadedTexture.dissolve)
            {
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.dissolve);
                *_dissolveSampler = 4;
                *_dissolveEnable = true;
            }
            else
            {
                *_dissolveEnable = false;
            }

            if (_bumpSampler && loadedTexture.bump)
            {
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.bump);
                *_bumpSampler = 5;
                *_bumpEnable = true;
            }
            else
            {
                *_bumpEnable = false;
            }
        }

    private:
        glv::UniformDeclaration _ambiantSampler;
        glv::UniformDeclaration _diffuseSampler;
        glv::UniformDeclaration _specularSampler;
        glv::UniformDeclaration _specularCoeffSampler;
        glv::UniformDeclaration _dissolveSampler;
        glv::UniformDeclaration _bumpSampler;

        glv::UniformDeclaration _ambiantEnable;
        glv::UniformDeclaration _diffuseEnable;
        glv::UniformDeclaration _specularEnable;
        glv::UniformDeclaration _specularCoeffEnable;
        glv::UniformDeclaration _dissolveEnable;
        glv::UniformDeclaration _bumpEnable;
    } _uniformTexture;


    std::vector<LoadedMaterial> _materials;
};

class GlslViewer
{
public:

    using LoadFile = glv::OperationResult;

    GlslViewer(int argc, char **argv) : failure(false)
    {
        const char *objFilename = 0;
        std::string vertexShader = defaultVertexShader;
        std::string fragmentShader = defaultFragmentShader;
        for (int i = 1; good() && i < argc; ++i)
        {
            if  (endsWith(argv[i], ".obj"))
                objFilename = argv[i];
            else if  (endsWith(argv[i], ".vert"))
                check(readFile(argv[i], vertexShader), std::string("loading '") + argv[i] + "'");
            else if (endsWith(argv[i], ".frag"))
                check(readFile(argv[i], fragmentShader), std::string("loading '") + argv[i] + "'");
            else
                std::cerr << "! Unknown argument '" << argv[i] << "'. Expecting *.obj, *.vert and/or *.frag file path." << std::endl;
        }
        if (good()) createProgram(vertexShader, fragmentShader);
        if (good()) createMesh(objFilename);
        if (good()) defineVertexAttributes();
    }

    LoadFile readFile(const char *filename, std::string &content)
    {
        sys::Duration duration;
        std::ifstream is(filename);
        std::string tmpContent;
        std::getline(is, tmpContent, '\0');
        if (!is.eof() && is.fail())
        {
            return LoadFile::failed("Cannot read file (maybe the path is wrong)!", duration.elapsed());
        }
        if (tmpContent.empty())
        {
            return LoadFile::failed("File is empty!", duration.elapsed());
        }
        content = std::move(tmpContent);
        return LoadFile::succeeded(duration.elapsed());
    }

    void createMesh(const char *objFilename)
    {
        vfm::ObjModel model;
        if(objFilename)
        {
            sys::Duration loadfileDuration;
            std::ifstream is(objFilename);
            if(! (is >> model))
            {
                check(LoadFile::failed("Cannot read file (maybe the path is wrong)!", 0), std::string("loading '") + objFilename + "'");
                return;
            }
            check(LoadFile::succeeded(loadfileDuration.elapsed()), std::string("loading '") + objFilename + "'");
        }
        else
        {
            std::istringstream modelStream(defaultMesh);
            modelStream >> model;
        }

        if (model.normals.empty())
        {
            model.computeNormals();
        }

        materialHandler.loadMaterials(textureLoader, objFilename, model);

        check(mesh.generate(model), "generating mesh");
    }

    void createProgram(const std::string &vertexShader, const std::string &fragmentShader)
    {
        glv::Shader vs(glv::ShaderType::VERTEX_SHADER);
        check(vs.compile(vertexShader), "compiling vertex shader");

        glv::Shader fs(glv::ShaderType::FRAGMENT_SHADER);
        check(fs.compile(fragmentShader), "compiling fragment shader");

        if (good())
        {
            check(program.attach(vs), "attaching vertex shader to GLSL program");
            check(program.attach(fs), "attaching fragment shader to GLSL program");
            check(program.link(), "linking GLSL program");
        }

        if(good())
        {
            program.use();

            timeUniform = program.getActiveUniform("time");
            mouseUniform = program.getActiveUniform("mouse");
            resolutionUniform = program.getActiveUniform("resolution");
            modelMatrixUniform = program.getActiveUniform("modelMat");
            viewMatrixUniform = program.getActiveUniform("viewMat");
            projectionMatrixUniform = program.getActiveUniform("projectionMat");
            mvMatrixUniform = program.getActiveUniform("mvMat");
            mvpMatrixUniform = program.getActiveUniform("mvpMat");
            normalMatrixUniform = program.getActiveUniform("normalMat");

            materialHandler.loadUniforms(program);
        }
    }

    bool defineVertexAttributes()
    {
        for (auto vertexAttributeDeclaration : program.getVertexAttributeDeclarations())
        {
            if(!check(this->mesh.defineVertexAttributeData(vertexAttributeDeclaration), "binding vertex attribute"))
            {
                return false;
            }
        }
        return true;
    }

    void update(glv::GlWindowContext& glf)
    {
        program.use();

        if (timeUniform)
        {
            *timeUniform = duration.elapsed() / 1000.0f;
        }

        glm::vec2 cursorPosition = glf.getCursorPosition();
        if(mouseUniform)
        {
            *mouseUniform = cursorPosition;
        }

        const glv::BoundingBox &boundingBox = mesh.getBoundingBox();
        glm::mat4x4 modelMatrix = glm::mat4x4(1.0f);

        glm::vec3 eyePosition {0,0,glm::distance(boundingBox.min, boundingBox.max) * 0.75f};
        glm::mat4x4 viewMatrix = glm::lookAt(eyePosition, glm::vec3(0,0,0), glm::normalize(glm::vec3(0,0.5,-0.5)));
        viewMatrix = glm::rotate(viewMatrix, cursorPosition.x * static_cast<float>(PI) * 4, glm::vec3(0,1,0));
        viewMatrix = glm::rotate(viewMatrix, cursorPosition.y * static_cast<float>(PI) * 4, glm::vec3(0,0,1));
        viewMatrix *= glm::translate(-boundingBox.center());

        glm::mat4x4 projectionMatrix = _camera.projectionMatrix();

        if(resolutionUniform)
        {
            *resolutionUniform = static_cast<glm::vec2>(_camera.viewport());
        }

        if(modelMatrixUniform)
        {
            *modelMatrixUniform = modelMatrix;
        }

        if(viewMatrixUniform)
        {
            *viewMatrixUniform = viewMatrix;
        }

        if(projectionMatrixUniform)
        {
            *projectionMatrixUniform = projectionMatrix;
        }

        if(mvMatrixUniform)
        {
            *mvMatrixUniform = viewMatrix * modelMatrix;
        }

        if(mvpMatrixUniform)
        {
            *mvpMatrixUniform = projectionMatrix * viewMatrix * modelMatrix;
        }

        if(normalMatrixUniform)
        {
            *normalMatrixUniform = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));
        }
        mesh.render(&materialHandler);
    }

    inline bool good() const
    {
        return !failure;
    }

    const glv::Camera & camera() const
    {
        return _camera;
    }

    glv::Camera & camera()
    {
        return _camera;
    }

private:
    bool check(const glv::OperationResult &r, const std::string &context)
    {
        if (r)
        {
            LOG(INFO) << context << " in " << r.duration() << "ms. " << r.message();
        }
        else
        {
            failure = true;
            LOG(WARNING) << context << " in " << r.duration() << "ms. " << r.message();
        }
        return r.ok();
    }

    bool failure;
    sys::Duration duration;
    glv::ShaderProgram program;
    glv::UniformDeclaration timeUniform;
    glv::UniformDeclaration mouseUniform;
    glv::UniformDeclaration resolutionUniform;
    glv::UniformDeclaration modelMatrixUniform;
    glv::UniformDeclaration viewMatrixUniform;
    glv::UniformDeclaration projectionMatrixUniform;
    glv::UniformDeclaration mvMatrixUniform;
    glv::UniformDeclaration mvpMatrixUniform;
    glv::UniformDeclaration normalMatrixUniform;
    glv::GlMesh mesh;
    MaterialHandler materialHandler;
    TextureLoader textureLoader;
    glv::PerspectiveCamera _camera;
};

int main(int argc, char **argv)
{
    sys::initLogger();
    glv::GlWindowContext glwc;
    LOG(INFO) << APP_NAME " by " APP_AUTHOR " (v" APP_VERSION " compilation date " APP_COMPILATION_DATE ")";

    if(!glwc.init("GLSL viewer", 800, 600) || ! glwc.makeCurrent())
    {
        LOG(FATAL) << "Cannot initialise OpenGL context!";
        return 1;
    }

    LOG(INFO) << "OpenGL vendor is " << glGetString(GL_VENDOR);
    LOG(INFO) << "OpenGL renderer is " << glGetString(GL_RENDERER);
    LOG(INFO) << "OpenGL version " << glGetString(GL_VERSION);
    LOG(INFO) << "OpenGLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION);
    {
        GlslViewer viewer(argc, argv);

        if (viewer.good())
        {
            auto setViewport = std::bind(&glv::Viewport::set, &viewer.camera().viewport(), std::placeholders::_1, std::placeholders::_2);
            glwc.setWindowSizeCallback(setViewport);

            glClearColor(0.5f,0.5f,0.5f,1.0f);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            /* Loop until the user closes the window */
            while (glwc.shouldContinue())
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                viewer.update(glwc);
                glwc.swapAndPollEvents();
            }
        }
    }
    return 0;
}

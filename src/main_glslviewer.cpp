#define GLM_FORCE_RADIANS
#include <iostream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <map>
#include <set>
#include "gl.hpp"
#include "SOIL.h"
#include "glm/gtx/transform.hpp"
#include "GlWindowContext.hpp"
#include "Path.hpp"
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlMesh.hpp"

bool endsWith (const char *base, const char *str) {
    int blen = std::strlen(base);
    int slen = std::strlen(str);
    return (blen >= slen) && (0 == std::strcmp(base + blen - slen, str));
}

const char defaultMesh[] =
        "v -1 -1 0\n"
        "v  1 -1 0\n"
        "v  1  1 0\n"
        "v -1  1 0\n"
        "f  1 2 3 4";

const char defaultVertexShader[] =
        GLSL_VERSION_HEADER
        "in vec2 position;\n"
        "out vec2 surfacePosition;\n"
        "void main(){\n"
        "  gl_Position = vec4(position, 0, 1);\n"
        "  surfacePosition = position;\n"
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

    virtual ~MaterialHandler()
    {
        std::set<GLuint> set;
        for(std::vector<LoadedMaterial>::iterator it = _materials.begin(); it < _materials.end(); ++it)
        {
            LoadedMaterial &loadedMaterial = *it;
            set.insert(loadedMaterial.texture.ambient);
            set.insert(loadedMaterial.texture.diffuse);
            set.insert(loadedMaterial.texture.specular);
            set.insert(loadedMaterial.texture.specularCoeff);
            set.insert(loadedMaterial.texture.dissolve);
            set.insert(loadedMaterial.texture.bump);
        }
        std::vector<GLuint> vector(set.begin(), set.end());
        glDeleteTextures(vector.size(), &vector[0]);
    }

    void loadUniforms(const glv::ShaderProgram &shaderProgram)
    {
        _uniformColor.load(shaderProgram);
        _uniformTexture.load(shaderProgram);
    }

    GLuint loadTexture(const sys::Path &basePath, const std::string &filename)
    {
        GLuint textureId = 0u;
        if (!filename.empty())
        {
            sys::Path filepath(basePath, filename.c_str());
            sys::Duration duration;
            textureId = SOIL_load_OGL_texture(filepath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS);
            if (textureId)
            {
                message(filepath, duration.elapsed());
            }
            else
            {
                warn(filepath, SOIL_last_result());
            }
        }
        return textureId;
    }

    void loadMaterials(const char *objFilename,  const vfm::ObjModel &model)
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
                isMat >> materialMaps[*libraryName];
                if(!isMat.eof() && isMat.fail())
                {
                    warn(mtlfile, "Cannot read file (maybe the path is wrong)!");
                    materialMaps.erase(*libraryName);
                }
                else
                {
                    message(mtlfile, loadfileDuration.elapsed());
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

                        // TODO load texture file once and only once

                        loadedMaterial.texture.ambient = loadTexture(basePath, material.map.ambient);
                        loadedMaterial.texture.diffuse = loadTexture(basePath, material.map.diffuse);
                        loadedMaterial.texture.specular = loadTexture(basePath, material.map.specular);
                        loadedMaterial.texture.specularCoeff = loadTexture(basePath, material.map.specularCoeff);
                        loadedMaterial.texture.dissolve = loadTexture(basePath, material.map.dissolve);
                        loadedMaterial.texture.bump = loadTexture(basePath, material.map.bump);
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
        std::cerr << "! " << "error while loading '" << filename << "':" << std::endl << "  " << message << std::endl;
    }

    void message(const char *filename, unsigned long duration)
    {
        std::cout << "* " << "loading '" << filename << "' in " << duration << "ms." << std::endl << std::endl;
    }

    class
    {
    public:

        void load(const glv::ShaderProgram &shaderProgram)
        {
            _ambiant = shaderProgram.getActiveUniform("material.ambient");
            _diffuse = shaderProgram.getActiveUniform("material.diffuse");
            _specular = shaderProgram.getActiveUniform("material.specular");
            _specularCoeff = shaderProgram.getActiveUniform("material.specularShininess");
        }

        void use(const vfm::Color &color)
        {
            if (_ambiant)
            {
                *_ambiant = color.ambient;
            }
            if (_diffuse)
            {
                *_diffuse = color.diffuse;
            }
            if (_specular)
            {
                *_specular = color.specular;
            }
            if (_specularCoeff)
            {
                *_specularCoeff = color.specularCoeff;
            }
        }

    private:
        glv::UniformDeclaration _ambiant;
        glv::UniformDeclaration _diffuse;
        glv::UniformDeclaration _specular;
        glv::UniformDeclaration _specularCoeff;
    } _uniformColor;

    class
    {
    public:

        void load(const glv::ShaderProgram &shaderProgram)
        {
            _ambiant = shaderProgram.getActiveUniform("sampler.ambient");
            _diffuse = shaderProgram.getActiveUniform("sampler.diffuse");
            _specular = shaderProgram.getActiveUniform("sampler.specular");
            _specularCoeff = shaderProgram.getActiveUniform("sampler.specularShininess");
            _dissolve = shaderProgram.getActiveUniform("sampler.dissolve");
            _bump = shaderProgram.getActiveUniform("sampler.bump");
        }

        inline bool hasTexture() const
        {
            return _ambiant || _diffuse || _specular || _specularCoeff || _dissolve || _bump;
        }

        void use(const LoadedTexture &loadedTexture)
        {
            if (_ambiant)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.ambient);
                *_ambiant = 0;
            }
            if (_diffuse)
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.diffuse);
                *_diffuse = 1;
            }
            if (_specular)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.specular);
                *_specular = 2;
            }
            if (_specularCoeff)
            {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.specularCoeff);
                *_specularCoeff = 3;
            }
            if (_dissolve)
            {
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.dissolve);
                *_dissolve = 4;
            }
            if (_bump)
            {
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, loadedTexture.bump);
                *_bump = 5;
            }
        }

    private:
        glv::UniformDeclaration _ambiant;
        glv::UniformDeclaration _diffuse;
        glv::UniformDeclaration _specular;
        glv::UniformDeclaration _specularCoeff;
        glv::UniformDeclaration _dissolve;
        glv::UniformDeclaration _bump;
    } _uniformTexture;


    std::vector<LoadedMaterial> _materials;
};

class GlslViewer
{
public:

    typedef glv::OperationResult LoadFileResult;

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

    LoadFileResult readFile(const char *filename, std::string &content)
    {
        sys::Duration duration;
        std::ifstream is(filename);
        std::string tmpContent;
        std::getline(is, tmpContent, '\0');
        if (!is.eof() && is.fail())
        {
            return LoadFileResult(false, "Cannot read file (maybe the path is wrong)!", duration.elapsed());
        }
        if (tmpContent.empty())
        {
            return LoadFileResult(false, "File is empty!", duration.elapsed());
        }
        content = tmpContent;
        return LoadFileResult(true, "", duration.elapsed());
    }

    void createMesh(const char *objFilename)
    {
        vfm::ObjModel model;
        if(objFilename)
        {
            sys::Duration loadfileDuration;
            std::ifstream is(objFilename);
            is >> model;
            if(!is.eof() && is.fail())
            {
                check(LoadFileResult(false, "Cannot read file (maybe the path is wrong)!", 0), std::string("loading '") + objFilename + "'");
                return;
            }
            else
            {
                check(LoadFileResult(true, "", loadfileDuration.elapsed()), std::string("loading '") + objFilename + "'");
            }
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

        materialHandler.loadMaterials(objFilename, model);

        check(mesh.generate(model), "generating mesh");
    }

    void createProgram(const std::string &vertexShader, const std::string &fragmentShader)
    {
        glv::Shader vs(glv::Shader::VERTEX_SHADER);
        check(vs.compile(vertexShader), "compiling vertex shader");

        glv::Shader fs(glv::Shader::FRAGMENT_SHADER);
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
        glv::VertexAttributeDeclarationVector vadv;
        program.extractActive(vadv);
        for (glv::VertexAttributeDeclarationVector::iterator it = vadv.begin(); it != vadv.end(); ++it)
        {
            if(!check(this->mesh.defineVertexAttributeData(*it), "binding vertex attribute"))
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

        glm::vec2 windowSize = glf.getWindowSize();
        const glv::BoundingBox &boundingBox = mesh.getBoundingBox();
        glm::mat4x4 modelMatrix = glm::translate(-boundingBox.center());
        float distance = glm::distance(boundingBox.min, boundingBox.max) * 0.75f;
        glm::mat4x4 viewMatrix = glm::lookAt(glm::vec3(0,0,distance), glm::vec3(0,0,0), glm::normalize(glm::vec3(0,0.5,-0.5)));
        viewMatrix = glm::rotate(viewMatrix, cursorPosition.x * static_cast<float>(M_PI) * 4, glm::vec3(0,1,0));
        viewMatrix = glm::rotate(viewMatrix, cursorPosition.y * static_cast<float>(M_PI) * 4, glm::vec3(0,0,1));
        glm::mat4x4 projectionMatrix = glm::perspectiveFov(static_cast<float>(70.0/180.0 * M_PI), windowSize.x, windowSize.y, .5f, distance*10.0f);

        if(resolutionUniform)
        {
            *resolutionUniform = windowSize;
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

private:
    bool check(const glv::OperationResult &r, const std::string &context)
    {
        if (r)
        {
            std::cout << "* " << context << " in " << r.duration() << "ms." << std::endl << "  " << r.message() << std::endl;
        }
        else
        {
            failure = true;
            std::cerr << "! " << context << ":" << std::endl << "  " << r.message() << std::endl;
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
};

int main(int argc, char **argv)
{
    glv::GlWindowContext glwc;
    if(!glwc.init("GLSL viewer", 800, 600) || ! glwc.makeCurrent())
    {
        std::cerr << "Cannot initialise OpenGL context" << std::endl;
        return false;
    }

    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    {
        GlslViewer viewer(argc, argv);

        if (viewer.good())
        {
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

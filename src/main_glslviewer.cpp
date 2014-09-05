#define GLM_FORCE_RADIANS
#include <iostream>
#include <sstream>
#include <cstring>
#include <cmath>
#include "gl.hpp"
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
        if (good()) createMesh(objFilename);
        if (good()) createProgram(vertexShader, fragmentShader);
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

        std::map<std::string, vfm::MaterialMap> materialMaps;
        loadMaterialMaps(objFilename, model, materialMaps);

        check(mesh.generate(model), "generating mesh");
    }

    void loadMaterialMaps(const char *objFilename,  vfm::ObjModel &model, std::map<std::string, vfm::MaterialMap> &materialMaps)
    {
        sys::Path objFilepath(objFilename);
        sys::Path currentPath = objFilepath.dirpath();
        std::string defaultMaterialLibrary = std::string(objFilepath.withoutExtension()) + ".mtl";
        for(vfm::ObjectVector::iterator objit = model.objects.begin(); objit != model.objects.end(); ++objit)
        {
            for(vfm::MaterialActivationVector::iterator matit = objit->materialActivations.begin(); matit != objit->materialActivations.end(); ++matit)
            {
                if (matit->materialLibrary.empty())
                {
                    matit->materialLibrary = defaultMaterialLibrary;
                }
                if (materialMaps.find(matit->materialLibrary) == materialMaps.end())
                {
                    sys::Path mtlfile(currentPath, matit->materialLibrary.c_str());
                    sys::Duration loadfileDuration;
                    std::ifstream isMat(mtlfile);
                    isMat >> materialMaps[matit->materialLibrary];
                    if(!isMat.eof() && isMat.fail())
                    {
                        check(LoadFileResult(false, "Cannot read file (maybe the path is wrong)!", 0), std::string("loading '") + static_cast<const char*>(mtlfile) + "'");
                        materialMaps.erase(matit->materialLibrary);
                    }
                    else
                    {
                        check(LoadFileResult(true, "", loadfileDuration.elapsed()), std::string("loading '") + static_cast<const char*>(mtlfile) + "'");
                    }
                }
            }
        }
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
        mesh.render();
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

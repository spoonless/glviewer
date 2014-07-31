#include <iostream>
#include <sstream>
#include <cstring>
#include "gl.hpp"
#include "GlWindowContext.hpp"
#include "Duration.hpp"
#include "ShaderProgram.hpp"
#include "GlMesh.hpp"

static bool trace(const glv::OperationResult &r, const std::string &context)
{
    if (r)
    {
        std::cout << "* " << context << " in " << r.duration() << "ms." << std::endl << r.message() << std::endl;
    }
    else
    {
        std::cerr << "! " << context << ":" << std::endl << r.message() << std::endl;
    }
    return r.ok();
}

bool endsWith (const char *base, const char *str) {
    int blen = std::strlen(base);
    int slen = std::strlen(str);
    return (blen >= slen) && (0 == std::strcmp(base + blen - slen, str));
}

const char defaultMesh[] =
        "v -1 -1 0\n"
        "v -1  1 0\n"
        "v  1  1 0\n"
        "v  1 -1 0\n"
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

    GlslViewer(int argc, char **argv)
    {
        std::string mesh = defaultMesh;
        std::string vertexShader = defaultVertexShader;
        std::string fragmentShader = defaultFragmentShader;
        for (int i = 1; i < argc; ++i)
        {
            if  (endsWith(argv[i], ".obj"))
                trace(readFile(argv[i], mesh), std::string("loading '") + argv[i] + "'");
            else if  (endsWith(argv[i], ".vert"))
                trace(readFile(argv[i], vertexShader), std::string("loading '") + argv[i] + "'");
            else if (endsWith(argv[i], ".frag"))
                trace(readFile(argv[i], fragmentShader), std::string("loading '") + argv[i] + "'");
            else
                std::cerr << "! Unknown argument '" << argv[i] << "'. Expecting *.obj, *.vert and/or *.frag file path." << std::endl;
        }
        createMesh(mesh);
        createProgram(vertexShader, fragmentShader);
        defineVertexAttributes();
    }

    LoadFileResult readFile(const char *filename, std::string &content)
    {
        Duration duration;
        std::ifstream is(filename);
        std::string tmpContent;
        std::getline(is, tmpContent, '\0');
        if (is.fail() && !is.eof())
        {
            return LoadFileResult(false, std::string("Cannot read '") + filename + "'!", duration.elapsed());
        }
        if (content.empty())
        {
            return LoadFileResult(false, std::string("File '") + filename + "' is empty !", duration.elapsed());
        }
        content = tmpContent;
        return LoadFileResult(true, "", duration.elapsed());
    }

    void createMesh(const std::string &objectMesh)
    {
        vfm::ObjModel model;
        std::istringstream plane(objectMesh);

        plane >> model;
        trace(mesh.generate(model), "generating mesh");
    }

    void createProgram(const std::string &vertexShader, const std::string &fragmentShader)
    {
        glv::Shader vs(glv::Shader::VERTEX_SHADER);
        trace(vs.compile(vertexShader), "compiling vertex shader");

        glv::Shader fs(glv::Shader::FRAGMENT_SHADER);
        trace(fs.compile(fragmentShader), "compiling fragment shader");

        trace(program.attach(vs), "attaching vertex shader to GLSL program");
        trace(program.attach(fs), "attaching fragment shader to GLSL program");
        trace(program.link(), "linking GLSL program");

        program.use();

        timeUniform = program.getActiveUniform("time");
        mouseUniform = program.getActiveUniform("mouse");
        resolutionUniform = program.getActiveUniform("resolution");
    }

    bool defineVertexAttributes()
    {
        glv::VertexAttributeDeclarationVector vadv;
        program.extractActive(vadv);
        for (glv::VertexAttributeDeclarationVector::iterator it = vadv.begin(); it != vadv.end(); ++it)
        {
            if(!trace(this->mesh.defineVertexAttributeData(*it), "binding vertex attribute"))
            {
                return false;
            }
        }
        return true;
    }

    void operator()(glv::GlWindowContext& glf)
    {
        program.use();

        if (timeUniform)
        {
            *timeUniform = duration.elapsed() / 1000.0f;
        }

        if(mouseUniform)
        {
            *mouseUniform = glf.getCursorPosition();
        }

        if(resolutionUniform)
        {
            *resolutionUniform = glf.getWindowSize();
        }

        mesh.render();
    }

private:
    Duration duration;
    glv::ShaderProgram program;
    glv::UniformDeclaration timeUniform;
    glv::UniformDeclaration mouseUniform;
    glv::UniformDeclaration resolutionUniform;
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

        /* Loop until the user closes the window */
        while (glwc.shouldContinue())
        {
            viewer(glwc);
            glwc.swapAndPollEvents();
        }
    }
    return 0;
}

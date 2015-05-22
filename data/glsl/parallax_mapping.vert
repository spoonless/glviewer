#version 330

/***************************************************/
/* vertex attributes                               */
/***************************************************/

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexTextureCoord;

/***************************************************/
/* out variables                                   */
/***************************************************/

const uint nbLightSources = 2u;

smooth out vec3 fragLightDir[nbLightSources];
smooth out vec3 fragViewDir;
smooth out vec2 fragTextureCoord;

/***************************************************/
/* Light sources definition view space coordinates */
/* Directional light sources have position.w = 0   */
/* and the position must be normalized.            */
/***************************************************/

struct LightSource
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform LightSource lightSources[nbLightSources] = {
    {
        vec4(0,0,1,1),
        vec3(.08),
        vec3(.8),
        vec3(.8)
    },
    {
        vec4(1,1,-1,0),
        vec3(.05),
        vec3(.5),
        vec3(.5)
    }
};

/***************************************************/
/* matrices                                        */
/***************************************************/

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

vec3 computeLightVector(in uint lightSourceNumber, in vec3 position)
{
    if(lightSources[lightSourceNumber].position.w == .0)
    {
        return normalize(lightSources[lightSourceNumber].position.xyz);
    }
    else
    {
        return normalize(lightSources[lightSourceNumber].position.xyz - position);
    }
}

void main()
{
    vec3 eyeNormal = normalize(normalMat * vertexNormal);
    vec3 eyeTangent = normalize(normalMat * vec3(vertexTangent));
    vec3 eyeBinormal = normalize(cross(eyeNormal, eyeTangent)) * vertexTangent.w;

    vec3 fragPosition = vec3(mvMat * vec4(vertexPosition,1.0));

    mat3 toTangentSpace = mat3(
        eyeTangent.x, eyeBinormal.x, eyeNormal.x,
        eyeTangent.y, eyeBinormal.y, eyeNormal.y,
        eyeTangent.z, eyeBinormal.z, eyeNormal.z
    );

    for (uint lightSourceNumber = 0u; lightSourceNumber < nbLightSources; ++lightSourceNumber)
    {
        vec3 eyeLightDir = computeLightVector(lightSourceNumber, fragPosition);
        fragLightDir[lightSourceNumber] = toTangentSpace * eyeLightDir;
    }

    fragViewDir = toTangentSpace * -fragPosition;
    fragTextureCoord = vertexTextureCoord;
    gl_Position = mvpMat * vec4(vertexPosition,1.0);
}

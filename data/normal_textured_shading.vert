#version 330
in vec3 position;
in vec3 normal;
in vec4 tangent;
in vec2 textureCoord;

smooth out vec3 fragPosition;
smooth out vec3 fragLightDir;
smooth out vec3 fragViewDir;
smooth out vec2 fragTextureCoord;

struct Light
{
    vec4 position;
    vec3 color;
};

uniform Light light = Light(vec4(0,1,1,0), vec3(1.0));

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

void main()
{
    vec3 eyeNormal = normalize(normalMat * normal);
    vec3 eyeTangent = normalize(normalMat * vec3(tangent));
    vec3 eyeBinormal = normalize(cross(eyeNormal, eyeTangent)) * tangent.w;
    
    mat3 toTangentSpace = mat3(
        eyeTangent.x, eyeBinormal.x, eyeNormal.x,
        eyeTangent.y, eyeBinormal.y, eyeNormal.y,
        eyeTangent.z, eyeBinormal.z, eyeNormal.z
    );

    fragPosition = vec3(mvMat * vec4(position,1.0));
    vec3 eyeLightDir = light.position.w == .0 ? light.position.xyz : light.position.xyz - fragPosition;
    fragLightDir = normalize(toTangentSpace * eyeLightDir);
    fragViewDir = normalize(toTangentSpace * -fragPosition);
    fragTextureCoord = textureCoord;
    gl_Position = mvpMat * vec4(position,1.0);
}

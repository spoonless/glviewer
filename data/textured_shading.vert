#version 330
in vec3 position;
in vec3 normal;
in vec2 textureCoord;

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;
smooth out vec2 fragTextureCoord;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

void main()
{
    fragTextureCoord = textureCoord;
    fragNormal = normalize(normalMat * normal);
    fragPosition = vec3(mvMat * vec4(position,1.0));
    gl_Position = mvpMat * vec4(position,1.0);
}

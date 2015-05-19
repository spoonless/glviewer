#version 330
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTextureCoord;

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;
smooth out vec2 fragTextureCoord;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

void main()
{
    fragTextureCoord = vertexTextureCoord;
    fragNormal = normalize(normalMat * vertexNormal);
    fragPosition = vec3(mvMat * vec4(vertexPosition,1.0));
    gl_Position = mvpMat * vec4(vertexPosition,1.0);
}

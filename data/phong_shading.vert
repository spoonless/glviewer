#version 330
in vec3 vertexPosition;
in vec3 vertexNormal;

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

void main()
{
    fragNormal = normalize(normalMat * vertexNormal);
    fragPosition = vec3(mvMat * vec4(vertexPosition,1.0));
    gl_Position = mvpMat * vec4(vertexPosition,1.0);
}

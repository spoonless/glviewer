#version 330
in vec3 position;
in vec3 normal;

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 projectionMat;
uniform mat4 mvpMat;

void main()
{
    fragNormal = normalize(normalMat * normal);
    fragPosition = vec3(mvMat * vec4(position,1.0));
    gl_Position = mvpMat * vec4(position,1.0);
}

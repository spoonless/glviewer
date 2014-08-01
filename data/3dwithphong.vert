#version 420

uniform mat4 mvMat;
uniform mat4 mvpMat;
uniform mat3 normalMat;

in vec3 normal;
in vec3 position;

smooth out vec3 normalFrag;
smooth out vec3 positionFrag;

void main()
{
  positionFrag = (mvMat * vec4(position, 1.0)).xyz;
  normalFrag = normalize(normalMat * normal);
  gl_Position = mvpMat * vec4(position, 1.0);
}

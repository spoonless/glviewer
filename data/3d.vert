#version 330
in vec3 vertexPosition;
uniform mat4x4 mvpMat;
out vec2 surfacePosition;
void main(){
  gl_Position = mvpMat * vec4(vertexPosition, 1);
  surfacePosition = vertexPosition.xy;
}


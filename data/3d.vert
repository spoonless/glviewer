#version 330
in vec3 position;
uniform mat4x4 mvpMat;
out vec2 surfacePosition;
void main(){
  gl_Position = mvpMat * vec4(position, 1);
  surfacePosition = position.xy;
}


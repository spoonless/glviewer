#version 330

smooth in vec3 lightIntensity;
out vec4 FragColor;

void main() {
  FragColor = vec4(lightIntensity, 1.0);
}

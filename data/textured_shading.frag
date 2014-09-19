#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragTextureCoord;

out vec4 fragColor;

struct Sampler
{
    sampler2D diffuse;
};

uniform Sampler sampler;

void main() {
    vec4 textureColor = texture(sampler.diffuse, fragTextureCoord);
    fragColor = textureColor;
}

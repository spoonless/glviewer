#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragTextureCoord;

out vec4 fragColor;

struct Texture
{
    sampler2D diffuse;
};

uniform Texture texture;

void main() {
    vec4 textureColor = texture(texture.diffuse, fragTextureCoord);
    fragColor = textureColor;
}

#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragLightDir;
smooth in vec3 fragViewDir;
smooth in vec2 fragTextureCoord;

out vec4 fragColor;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specularShininess;
};

struct Texture2D
{
    bool enable;
    sampler2D sampler;
};

struct MaterialTexture
{
    Texture2D ambient;
    Texture2D diffuse;
    Texture2D specular;
    Texture2D bump;
};

struct Light
{
    vec4 position;
    vec3 color;
};

uniform Light light = Light(vec4(0,1,1,0), vec3(1.0));

uniform Material material;

uniform MaterialTexture materialTexture;

void computeColors(in vec3 normal, inout vec3 ambientCoeff, inout vec3 diffuseCoeff, inout vec3 specularCoeff)
{
    vec3 r = reflect(-fragLightDir, normal);

    float cos_sn = max(dot(fragLightDir, normal), 0.0);

    ambientCoeff += light.color;
    diffuseCoeff += light.color * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        specularCoeff += light.color * pow(max(dot(r,fragViewDir), .0), material.specularShininess*2);
    }
}

void main() {
    vec3 ambientCoeff = vec3(.0);
    vec3 diffuseCoeff = vec3(.0);
    vec3 specularCoeff = vec3(.0);
    vec3 ambientColor = materialTexture.ambient.enable ? texture(materialTexture.ambient.sampler, fragTextureCoord).xyz : material.ambient;
    vec3 diffuseColor = materialTexture.diffuse.enable ? texture(materialTexture.diffuse.sampler, fragTextureCoord).xyz : material.diffuse;
    vec3 specularColor = materialTexture.specular.enable ? texture(materialTexture.specular.sampler, fragTextureCoord).xyz : material.specular;
    vec3 normal = vec3(.0,.0,1.0);
    if (materialTexture.bump.enable)
    {
        normal = 2.0 * texture(materialTexture.bump.sampler, fragTextureCoord).xyz - 1;
    }

    computeColors(normal, ambientCoeff, diffuseCoeff, specularCoeff);

    fragColor = vec4((ambientCoeff * ambientColor) + (diffuseCoeff * diffuseColor) + (specularCoeff * diffuseColor), 1.0);
}

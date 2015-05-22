#version 330

#define WITH_PARALLAX_LIMITED
#define ANIMATE

smooth in vec3 fragLightDir;
smooth in vec3 fragViewDir;
smooth in vec2 fragTextureCoord;

uniform float time;
uniform float parallaxScale = .05;

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
    Texture2D normalMapping;
    Texture2D displacement;
};

struct Light
{
    vec4 position;
    vec3 color;
};

uniform Light light = {vec4(0,1,1,0), vec3(1.0)};

uniform Material material;

uniform MaterialTexture materialTexture;

float animationKeyTime = max(.0, sqrt(cos(time)));

void computeColors(in vec3 normal, in vec3 viewDir, in vec3 lightDir, inout vec3 ambientCoeff, inout vec3 diffuseCoeff, inout vec3 specularCoeff)
{

    float cos_sn = max(dot(lightDir, normal), 0.0);
    ambientCoeff += light.color;
    diffuseCoeff += light.color * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        vec3 r = reflect(-lightDir, normal);
        specularCoeff += light.color * pow(max(dot(r,viewDir), .0), material.specularShininess);
    }
}

vec2 parallaxMappingTextureCoord(in vec3 viewDir)
{
    if (! materialTexture.displacement.enable)
    {
        return fragTextureCoord;
    }
    float initialHeight = 1 - texture(materialTexture.displacement.sampler, fragTextureCoord).x;

#ifdef WITH_PARALLAX_LIMITED
    // calculate amount of offset for Parallax Mapping With Offset Limiting
    vec2 texCoordOffset = parallaxScale * viewDir.xy * initialHeight;
#else
    // calculate amount of offset for Parallax Mapping
    vec2 texCoordOffset = parallaxScale * viewDir.xy / viewDir.z * initialHeight;// * viewDir.z * dot(viewDir, vec3(0,0,1));
#endif

#ifdef ANIMATE
    return mix(fragTextureCoord, fragTextureCoord - texCoordOffset, animationKeyTime);
#else
    return fragTextureCoord - texCoordOffset;
#endif
}

void main() {
    vec3 ambientCoeff = vec3(.0);
    vec3 diffuseCoeff = vec3(.0);
    vec3 specularCoeff = vec3(.0);
    vec3 normal = vec3(.0,.0,1.0);
    vec3 viewDir = normalize(fragViewDir);
    vec3 lightDir = normalize(fragLightDir);
    vec2 textureCoord = parallaxMappingTextureCoord(viewDir);
    vec3 ambientColor = materialTexture.ambient.enable ? texture(materialTexture.ambient.sampler, textureCoord).xyz : material.ambient;
    vec3 diffuseColor = materialTexture.diffuse.enable ? texture(materialTexture.diffuse.sampler, textureCoord).xyz : material.diffuse;
    vec3 specularColor = materialTexture.specular.enable ? texture(materialTexture.specular.sampler, textureCoord).xyz : material.specular;

    if (materialTexture.normalMapping.enable)
    {
        vec3 mapNormal = 2 * texture(materialTexture.normalMapping.sampler, textureCoord).xyz - 1;
#ifdef ANIMATE
        normal = normalize(mix(normal,mapNormal, animationKeyTime));
#else
        normal = normalize(mapNormal);
#endif
    }

    computeColors(normal, viewDir, lightDir, ambientCoeff, diffuseCoeff, specularCoeff);

    fragColor = vec4((ambientCoeff * ambientColor) + (diffuseCoeff * diffuseColor) + (specularCoeff * diffuseColor), 1.0);
}

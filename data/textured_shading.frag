#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
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
};

/*
  Light source position must be in view space coordinates.
  Directional light source is indicated by a w component
  equals to 0 and the position must be normalized.
*/
struct LightSource
{
    vec4 position;
    vec3 color;
};

const uint nbLights = 3u;

uniform vec3 ambientColor = vec3(0.05);

uniform LightSource lightSources[nbLights] = {
    LightSource(normalize(vec4(0,1,1,0)), vec3(.35,.4,.4)),
    LightSource(normalize(vec4(0,0,0,-10)), vec3(0.6,0.6,.55)),
    LightSource(vec4(20,0,0,1), vec3(.4,.35,.4))
};

uniform Material material;

uniform MaterialTexture materialTexture;

void computeColors(in uint lightIndex, inout vec3 ambientCoeff, inout vec3 diffuseCoeff, inout vec3 specularCoeff)
{
    vec3 n = normalize(fragNormal);
    vec3 s;
    if (lightSources[lightIndex].position.w == .0)
    {
        s = lightSources[lightIndex].position.xyz;
    }
    else
    {
        s = normalize(lightSources[lightIndex].position.xyz - fragPosition);
    }
    float cos_sn = max(dot(s, n), 0.0);

    ambientCoeff += lightSources[lightIndex].color;
    diffuseCoeff += lightSources[lightIndex].color * cos_sn;

    if (cos_sn > .0 && material.specularShininess > .0)
    {
        vec3 v = normalize(-fragPosition);
        vec3 r = reflect(-s, n);
        specularCoeff += lightSources[lightIndex].color * pow(max(dot(r,v), .0), material.specularShininess);
    }
}

void main() {
    vec3 ambientCoeff = vec3(.0);
    vec3 diffuseCoeff = vec3(.0);
    vec3 specularCoeff = vec3(.0);
    vec3 ambientColor = materialTexture.ambient.enable ? texture(materialTexture.ambient.sampler, fragTextureCoord).xyz : material.ambient;
    vec3 diffuseColor = materialTexture.diffuse.enable ? texture(materialTexture.diffuse.sampler, fragTextureCoord).xyz : material.diffuse;
    vec3 specularColor = materialTexture.specular.enable ? texture(materialTexture.specular.sampler, fragTextureCoord).xyz : material.specular;
    
    for (uint i = 0u; i < nbLights; ++i)
    {
        computeColors(i, ambientCoeff, diffuseCoeff, specularCoeff);
    }
    
    fragColor = vec4((ambientCoeff * ambientColor) + (diffuseCoeff * diffuseColor) + (specularCoeff * diffuseColor), 1.0);
}

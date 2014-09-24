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

struct LightSource
{
    vec4 position;
    vec3 color;
};

const uint nbLights = 2u;

uniform vec3 ambientColor = vec3(0.05);

uniform LightSource lightSources[nbLights] = {
    LightSource(vec4(0,1,1,0), vec3(.5)),
    LightSource(vec4(20,0,0,1), vec3(.5))
};

uniform Material material;

uniform MaterialTexture materialTexture;

void computeColors(in uint lightIndex, inout vec3 ambientCoeff, inout vec3 diffuseCoeff, inout vec3 specularCoeff)
{
    vec3 n = normalize(fragNormal);
    vec3 s;
    if (lightSources[lightIndex].position.w == .0)
    {
        s = normalize(lightSources[lightIndex].position.xyz);
    }
    else
    {
        s = normalize(lightSources[lightIndex].position.xyz - fragPosition);
    }
    vec3 v = normalize(-fragPosition);
    // using halfway vector instead of real reflection vector
    vec3 r = normalize(v+s);

    float cos_sn = max(dot(s, n), 0.0);

    ambientCoeff += lightSources[lightIndex].color;
    diffuseCoeff += lightSources[lightIndex].color * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        specularCoeff += lightSources[lightIndex].color * pow(max(dot(r,n), .0), material.specularShininess*2);
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

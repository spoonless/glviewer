#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragTextureCoord;

out vec4 fragColor;

struct Sampler
{
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
};

uniform Sampler sampler;

struct LightSource
{
    vec4 position;
    vec3 color;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specularShininess;
};


const float ambientFactor = 0.2;
const uint nbLights = 2u;
uniform LightSource lightSources[nbLights] = {
    {
        vec4(0,1,1,0),
        vec3(1)
    },
    {
        vec4(20,0,0,1),
        vec3(1)
    }
};

uniform vec3 ambientColor = vec3(0.05);

uniform Material material = {
    vec3(0),
    vec3(0),
    vec3(0),
    40
};

void computeColors(in uint lightIndex, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular)
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

    ambient += lightSources[lightIndex].color;
    diffuse += lightSources[lightIndex].color * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        specular += lightSources[lightIndex].color * pow(max(dot(r,n), .0), material.specularShininess*2);
    }
}

void main() {
    vec3 ambient = vec3(.0);
    vec3 diffuse = vec3(.0);
    vec3 specular = vec3(.0);
    vec4 ambientTexture = texture(sampler.ambient, fragTextureCoord);
    vec4 diffuseTexture = texture(sampler.diffuse, fragTextureCoord);
    vec4 specularTexture = texture(sampler.specular, fragTextureCoord);
    
    for (uint i = 0u; i < nbLights; ++i)
    {
        computeColors(i, ambient, diffuse, specular);
    }
    
    ambient = all(equal(ambientTexture, vec4(0.0))) ? material.ambient * ambient : ambientTexture.xyz * ambient;
    diffuse = all(equal(diffuseTexture, vec4(0.0))) ? material.diffuse * diffuse : diffuseTexture.xyz * diffuse;
    specular = all(equal(specularTexture, vec4(0.0))) ? material.specular * ambient : specularTexture.xyz * ambient;

    fragColor = vec4(diffuse, 1.0);
}

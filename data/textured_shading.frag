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
    vec3(1),
    vec3(1),
    vec3(1),
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

    ambient += lightSources[lightIndex].color * material.ambient;
    diffuse += lightSources[lightIndex].color * material.diffuse * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        specular += lightSources[lightIndex].color * material.specular * pow(max(dot(r,n), .0), material.specularShininess*2);
    }
}

void main() {
    vec3 textureColor = texture(sampler.diffuse, fragTextureCoord).rgb;
    vec3 ambient, diffuse, specular;
    for (uint i = 0u; i < nbLights; ++i)
    {
        computeColors(i, ambient, diffuse, specular);
    }

    fragColor = vec4(ambientFactor * textureColor * ambient + textureColor * diffuse + specular, 1.0);
}

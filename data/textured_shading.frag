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


const uint nbLights = 2u;
uniform LightSource lightSources[nbLights] = {
    {
        vec4(0,1,1,0),
        vec3(0.3)
    },
    {
        vec4(20,0,0,1),
        vec3(0.6)
    }
};

uniform vec3 ambientColor = vec3(0.05);

uniform Material material = {
    vec3(.1,.1,.1),
    vec3(.7),
    vec3(.6),
    40
};

vec3 phongModel(in vec4 lightPosition, in vec4 textureColor)
{
    vec3 n = normalize(fragNormal);
    vec3 s;
    if (lightPosition.w == .0)
    {
        s = normalize(lightPosition.xyz);
    }
    else
    {
        s = normalize(lightPosition.xyz - fragPosition);
    }
    vec3 v = normalize(-fragPosition);
    // using halfway vector instead of real reflection vector
    vec3 r = normalize(v+s);

    float cos_sn = max(dot(s, n), 0.0);

    vec3 diffuse = mix(textureColor.rgb, material.diffuse * cos_sn, .1);
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        vec3 specular = material.specular * 0.1 * pow(max(dot(r,n), .0), material.specularShininess*2);
        return diffuse + specular;
    }
    return diffuse;
}

void main() {
    vec4 textureColor = texture(sampler.diffuse, fragTextureCoord);
    vec3 color = vec3(0);
    for (uint i = 0u; i < nbLights; ++i)
    {
        color += lightSources[i].color * phongModel(lightSources[i].position, textureColor);
    }
    fragColor = vec4(color + ambientColor * material.ambient, 1.0);
}

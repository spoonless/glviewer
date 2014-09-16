#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;

out vec4 fragColor;

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

const uint nbLights = 2;
uniform LightSource lightSources[nbLights] = {
    {
        vec4(0,1,1,0),
        vec3(0.4)
    },
    {
        vec4(200,100,30,1),
        vec3(0.6)
    }
};

uniform vec3 ambientColor = vec3(1.0);

uniform Material material = {
    vec3(.1,.1,.1),
    vec3(.7),
    vec3(.6),
    40
};

vec3 phongModel(in vec4 lightPosition)
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
    vec3 r = reflect(-s, n);

    float cos_sn = max(dot(s, n), 0.0);

    vec3 diffuse = material.diffuse * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        vec3 specular = material.specular * pow(max(dot(r,v), .000000000000001), material.specularShininess);
        return diffuse + specular;
    }
    return diffuse;
}

void main() {
    vec3 lightIntensity = material.ambient * ambientColor;
    for (int i = 0; i < nbLights; ++i)
    {
        lightIntensity += lightSources[i].color * phongModel(lightSources[i].position);
    }
    fragColor = vec4(lightIntensity, 1.0);
}

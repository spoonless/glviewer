#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;

out vec4 fragColor;

struct FogInfo
{
    float minDistance;
    float maxDistance;
    vec3 color;
};

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

uniform FogInfo fogInfo = {.0, 10.0, {.5,.5,.5}};

const uint nbLights = 2u;
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

float computeFogFactor()
{
    float dist = abs(fragPosition.z);
    float fogFactor = (fogInfo.maxDistance - abs(fragPosition.z)) / (fogInfo.maxDistance - fogInfo.minDistance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

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
    // using halfway vector instead of real reflection vector
    vec3 r = normalize(v+s);

    float cos_sn = max(dot(s, n), 0.0);

    vec3 diffuse = material.diffuse * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        vec3 specular = material.specular * pow(max(dot(r,n), .0), material.specularShininess*2);
        return diffuse + specular;
    }
    return diffuse;
}

void main() {
    vec3 lightIntensity = material.ambient * ambientColor;
    float fogFactor = computeFogFactor();
    if (fogFactor < 1.0)
    {
        for (uint i = 0u; i < nbLights; ++i)
        {
            lightIntensity += lightSources[i].color * phongModel(lightSources[i].position);
        }
    }
    lightIntensity = mix(fogInfo.color, lightIntensity, fogFactor);

    fragColor = vec4(lightIntensity, 1.0);
}

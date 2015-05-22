#version 330

/***************************************************/
/* in variables                                    */
/***************************************************/

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragTextureCoord;

/***************************************************/
/* out variables                                   */
/***************************************************/

out vec4 fragColor;

/***************************************************/
/* Light sources definition view space coordinates */
/* Directional light sources have position.w = 0   */
/* and the position must be normalized.            */
/***************************************************/

struct LightSource
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const uint nbLightSources = 2u;
uniform LightSource lightSources[nbLightSources] = {
    {
        vec4(0,0,1,1),
        vec3(.08),
        vec3(.8),
        vec3(.8)
    },
    {
        vec4(1,1,-1,0),
        vec3(.05),
        vec3(.5),
        vec3(.5)
    }
};

/***************************************************/
/* Material definition                             */
/***************************************************/

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
};

uniform Material material = {
    vec3(.3),
    vec3(1),
    vec3(.1),
    16
};

uniform MaterialTexture materialTexture;

vec3 computeLightVector(in uint lightSourceNumber, in vec3 position)
{
    if(lightSources[lightSourceNumber].position.w == .0)
    {
        return normalize(lightSources[lightSourceNumber].position.xyz);
    }
    else
    {
        return normalize(lightSources[lightSourceNumber].position.xyz - position);
    }
}

vec3 applyLightningModel(in uint lightSourceNumber, in vec3 N, in vec3 V, in vec3 L, in vec3 Ka, in vec3 Kd, in vec3 Ks, in float Ns)
{
    vec3 ambient = lightSources[lightSourceNumber].ambient * Ka;

    float lambertian = dot(N, L);
    vec3 diffuse = lightSources[lightSourceNumber].diffuse * Kd * clamp(lambertian, 0, 1);

    if (lambertian > .0 && Ns > .0)
    {
        vec3 R = reflect(-L, N);
        float Ispec = pow(clamp(dot(V, R), 0, 1), Ns);
        vec3 specular = lightSources[lightSourceNumber].specular * Ks * Ispec;
        return ambient + diffuse + specular;
    }
    else
    {
        return ambient + diffuse;
    }
}

void main() {
    vec3 V = normalize(-fragPosition);
    vec3 Ka = materialTexture.ambient.enable ? texture(materialTexture.ambient.sampler, fragTextureCoord).xyz : material.ambient;
    vec3 Kd = materialTexture.diffuse.enable ? texture(materialTexture.diffuse.sampler, fragTextureCoord).xyz : material.diffuse;
    vec3 Ks = materialTexture.specular.enable ? texture(materialTexture.specular.sampler, fragTextureCoord).xyz : material.specular;

    vec3 N;
    if (materialTexture.normalMapping.enable)
    {
        N = texture2D(materialTexture.normalMapping.sampler, fragTextureCoord).xyz;
        N = normalize((2.0 * N) - 1.0);
    }
    else
    {
         N = normalize(fragNormal);
    }

    vec3 color = vec3(.0);
    for (uint lightSourceNumber = 0u; lightSourceNumber < nbLightSources; ++lightSourceNumber)
    {
        vec3 L = computeLightVector(lightSourceNumber, fragPosition);
        color += applyLightningModel(lightSourceNumber, N, V, L, Ka, Kd, Ks, material.specularShininess);
    }
    
    fragColor = vec4(color, 1);
}

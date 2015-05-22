#version 330

/***************************************************/
/* in variables                                    */
/***************************************************/

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;

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

uniform Material material = {
    vec3(.3),
    vec3(1),
    vec3(.1),
    16
};

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

vec3 applyLightningModel(in uint lightSourceNumber, in vec3 N, in vec3 V, in vec3 L)
{
    vec3 ambient = lightSources[lightSourceNumber].ambient * material.ambient;

    float lambertian = dot(N, L);
    vec3 diffuse = lightSources[lightSourceNumber].diffuse * material.diffuse * clamp(lambertian, 0, 1);

    if (lambertian > .0 && material.specularShininess > .0)
    {
        // using halfway vector instead of real reflection vector
        vec3 H = normalize(V+L);
        float Ispec = pow(clamp(dot(H, N), 0, 1), material.specularShininess*2);
        vec3 specular = lightSources[lightSourceNumber].specular * material.specular * Ispec;
        return ambient + diffuse + specular;
    }
    else
    {
        return ambient + diffuse;
    }
}

void main()
{
    vec3 V = normalize(-fragPosition);
    vec3 N = normalize(fragNormal);

    vec3 color = vec3(0);
    for (uint lightSourceNumber = 0u; lightSourceNumber < nbLightSources; ++lightSourceNumber)
    {
        vec3 L = computeLightVector(lightSourceNumber, fragPosition);
        color += applyLightningModel(lightSourceNumber, N, V, L);
    }

    fragColor = vec4(color, 1);
}

#version 330
/***************************************************/
/* vertex attributes                               */
/***************************************************/

in vec3 vertexPosition;
in vec3 vertexNormal;

/***************************************************/
/* out variables                                   */
/***************************************************/

smooth out vec3 color;

/***************************************************/
/* Light sources definition                        */
/* Directional light sources have position.w = 0   */
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

/***************************************************/
/* matrices                                        */
/***************************************************/

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

vec3 computeNormal()
{
    return normalize(normalMat * vertexNormal);
}

vec3 computeViewVector(in vec3 position)
{
    return normalize(-position);
}

vec3 computeLightVector(in uint lightSourceNumber, in vec3 position)
{
    if(lightSources[lightSourceNumber].position.w == .0)
    {
        return normalize(lightSources[lightSourceNumber].position.xyz);
    }
    else
    {
        return normalize(lightSources[lightSourceNumber].position.xyz -position);
    }
}

vec3 applyLightningModel(in uint lightSourceNumber, in vec3 N, in vec3 V, in vec3 L)
{
    vec3 ambient = lightSources[lightSourceNumber].ambient * material.ambient;

    float lambertian = dot(N, L);
    vec3 diffuse = lightSources[lightSourceNumber].diffuse * material.diffuse * clamp(lambertian, 0, 1);

    if (lambertian > .0 && material.specularShininess > .0)
    {
        vec3 R = reflect(-L, N);
        float Ispec = pow(clamp(dot(V, R), 0, 1), material.specularShininess);
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
    vec3 position = (mvMat * vec4(vertexPosition,1.0)).xyz;
    vec3 V = computeViewVector(position);
    vec3 N = computeNormal();

    color = vec3(0);
    for (uint lightSourceNumber = 0u; lightSourceNumber < nbLightSources; ++lightSourceNumber)
    {
        vec3 L = computeLightVector(lightSourceNumber, position);
        color += applyLightningModel(lightSourceNumber, N, V, L);
    }

    gl_Position = mvpMat * vec4(vertexPosition,1.0);
}

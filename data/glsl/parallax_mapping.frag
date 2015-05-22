#version 330

//#define WITH_PARALLAX_LIMITED
#define ANIMATE

/***************************************************/
/* in variables                                    */
/***************************************************/

const uint nbLightSources = 2u;

smooth in vec3 fragLightDir[nbLightSources];
smooth in vec3 fragViewDir;
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
    Texture2D specularShininess;
    Texture2D normalMapping;
    Texture2D displacement;
};

uniform Material material = {
    vec3(.3),
    vec3(1),
    vec3(.1),
    16
};

uniform MaterialTexture materialTexture;

/***************************************************/
/* Additional uniforms                             */
/***************************************************/

uniform float time;
uniform float parallaxScale = .04;

float animationKeyTime = clamp(10*cos(time)+.5, .0, 1.0);

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

vec2 parallaxMappingTextureCoord(in vec3 V)
{
    if (! materialTexture.displacement.enable)
    {
        return fragTextureCoord;
    }
    float initialHeight = 1 - texture(materialTexture.displacement.sampler, fragTextureCoord).x;

#ifdef WITH_PARALLAX_LIMITED
    // calculate amount of offset for Parallax Mapping With Offset Limiting
    vec2 texCoordOffset = parallaxScale * V.xy * initialHeight;
#else
    // calculate amount of offset for Parallax Mapping
    vec2 texCoordOffset = parallaxScale * V.xy / V.z * initialHeight;
#endif

#ifdef ANIMATE
    return mix(fragTextureCoord - texCoordOffset, fragTextureCoord, animationKeyTime);
#else
    return fragTextureCoord - texCoordOffset;
#endif
}

vec3 computeNormal(vec2 textureCoord)
{
    if (materialTexture.normalMapping.enable)
    {
        vec3 N = texture2D(materialTexture.normalMapping.sampler, textureCoord).xyz;
#ifdef ANIMATE
        N = normalize((2.0 * N) - 1.0);
        return mix(N, vec3(.0,.0,1.0), animationKeyTime);
#else
        return normalize((2.0 * N) - 1.0);
#endif
    }
    return vec3(.0,.0,1.0);
}

void main() {
    vec3 V = normalize(fragViewDir);
    vec2 textureCoord = parallaxMappingTextureCoord(fragViewDir);
    vec3 Ka = materialTexture.ambient.enable ? texture(materialTexture.ambient.sampler, textureCoord).xyz : material.ambient;
    vec3 Kd = materialTexture.diffuse.enable ? texture(materialTexture.diffuse.sampler, textureCoord).xyz : material.diffuse;
    vec3 Ks = materialTexture.specular.enable ? texture(materialTexture.specular.sampler, textureCoord).xyz : material.specular;
    float Ns = materialTexture.specularShininess.enable ? texture(materialTexture.specularShininess.sampler, textureCoord).x * material.specularShininess : material.specularShininess;
    vec3 N = computeNormal(textureCoord);

    vec3 color = vec3(.0);
    for (uint lightSourceNumber = 0u; lightSourceNumber < nbLightSources; ++lightSourceNumber)
    {
        vec3 L = normalize(fragLightDir[lightSourceNumber]);
        color += applyLightningModel(lightSourceNumber, N, V, L, Ka, Kd, Ks, Ns);
    }

    fragColor = vec4(color, 1);
}

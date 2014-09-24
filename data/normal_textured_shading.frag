#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragLightDir;
smooth in vec3 fragViewDir;
smooth in vec2 fragTextureCoord;

out vec4 fragColor;

struct Sampler
{
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D bump;
};

uniform Sampler sampler;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specularShininess;
};

struct Light
{
    vec4 position;
    vec3 color;
};

uniform Light light = Light(vec4(0,1,1,0), vec3(1.0));

const float ambientFactor = 0.2;
uniform vec3 ambientColor = vec3(1.0);

uniform Material material = {
    vec3(1),
    vec3(1),
    vec3(1),
    40
};

void computeColors(in vec3 normal, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular)
{
    vec3 r = reflect(-fragLightDir, normal);

    float cos_sn = max(dot(fragLightDir, normal), 0.0);

    ambient += light.color * material.ambient;
    diffuse += light.color * material.diffuse * cos_sn;
    if (cos_sn > .0 && material.specularShininess > .0)
    {
        // using specularShininess * 2 to correct halfway vector optimization
        specular += light.color * material.specular * pow(max(dot(r,fragViewDir), .0), material.specularShininess*2);
    }
}

void main() {
    vec3 ambient = vec3(0);
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    vec3 ambientTexture = texture(sampler.ambient, fragTextureCoord).rgb;
    vec3 diffuseTexture = texture(sampler.diffuse, fragTextureCoord).rgb;
    vec3 specularTexture = texture(sampler.specular, fragTextureCoord).rgb;

    vec4 normal = 2.0 * texture(sampler.bump, fragTextureCoord) - 1;

    computeColors(normal.xyz, ambient, diffuse, specular);

    fragColor = vec4((ambientFactor * ambientTexture * ambient) + (diffuseTexture * diffuse) + (specularTexture * specular), 1.0);
}

#version 330
in vec3 position;
in vec3 normal;

smooth out vec3 lightIntensity;

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

uniform Material material = {
    vec3(.2,.2,.2),
    vec3(.7),
    vec3(.2),
    2
};

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

vec3 phongModel(in vec4 lightPosition, in vec3 eyeNormal, in vec4 eyePosition)
{
  vec3 s;
  if (lightPosition.w == .0)
  {
      s = normalize(vec3(lightPosition));
  }
  else
  {
      s = normalize(vec3(lightPosition - eyePosition));
  }
  vec3 v = normalize(-eyePosition.xyz);
  vec3 r = reflect(-s, eyeNormal);

  float cos_sn = max(dot(s, eyeNormal), 0.0);

  vec3 diffuse = material.diffuse * cos_sn;
  if (cos_sn > .0 && material.specularShininess > .0)
  {
    vec3 specular = material.specular * pow(max(dot(r,v), .000000000000001), material.specularShininess);
    return material.ambient + diffuse + specular;
  }
  return material.ambient + diffuse;
}

void main()
{
  vec3 eyeNormal = normalize(normalMat * normal);
  vec4 eyePosition = mvMat * vec4(position,1.0);

  lightIntensity = vec3(.0);
  for (uint i = 0; i < nbLights; ++i)
  {
      lightIntensity += lightSources[i].color * phongModel(lightSources[i].position, eyeNormal, eyePosition);
  }

  gl_Position = mvpMat * vec4(position,1.0);
}

#version 330

const vec4 lightPosition[2] = vec4[2](vec4(0,1,1,0), vec4(200,100,30,1));
const vec3 lightColor[2] = vec3[2](vec3(0.4), vec3(0.6));

uniform vec3 materialAmbient = vec3(.2,.2,.2);
uniform vec3 materialDiffuse = vec3(.7);
uniform vec3 materialSpecular = vec3(.6);
uniform float materialShininess = 10;

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;

out vec4 fragColor;

vec3 phongModel(in vec4 lightPosition)
{
  vec3 s;
  if (lightPosition.w == .0)
  {
      s = normalize(lightPosition.xyz);
  }
  else
  {
      s = normalize(lightPosition.xyz - fragPosition);
  }
  vec3 v = normalize(-fragPosition.xyz);
  vec3 r = reflect(-s, fragNormal);

  float cos_sn = max(dot(s, fragNormal), 0.0);

  vec3 diffuse = materialDiffuse * cos_sn;
  if (cos_sn > .0 && materialShininess > .0)
  {
    vec3 specular = materialSpecular * pow(max(dot(r,v), .000000000000001), materialShininess);
    return materialAmbient + diffuse + specular;
  }
  return materialAmbient + diffuse;
}

void main() {
    vec3 lightIntensity = vec3(.0);
    for (int i = 0; i < 2; ++i)
    {
        lightIntensity += lightColor[i] * phongModel(lightPosition[i]);
    }
    fragColor = vec4(lightIntensity, 1.0);
}

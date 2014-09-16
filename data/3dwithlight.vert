#version 330
in vec3 position;
in vec3 normal;

smooth out vec3 lightIntensity;

uniform vec4 lightPosition = vec4(20,10,30,1);
uniform vec3 lightAmbient = vec3(0.3);
uniform vec3 lightDiffuse = vec3(1.0);
uniform vec3 lightSpecular = vec3(0.5);

uniform vec3 materialAmbient = vec3(.2,.2,.2);
uniform vec3 materialDiffuse = vec3(.7);
uniform vec3 materialSpecular = vec3(.2);
uniform float materialShininess = 2;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 projectionMat;
uniform mat4 mvpMat;

vec3 phongModel(in vec3 eyeNormal, in vec4 eyePosition)
{
  vec3 s = normalize(vec3(lightPosition - eyePosition));
  vec3 v = normalize(-eyePosition.xyz);
  vec3 r = reflect(-s, eyeNormal);

  float cos_sn = max(dot(s, eyeNormal), 0.0);

  vec3 ambient = lightAmbient * materialAmbient;
  vec3 diffuse = lightDiffuse * materialDiffuse * cos_sn;
  if (cos_sn > .0 && materialShininess > .0)
  {
    vec3 specular = lightSpecular * materialSpecular * pow(max(dot(r,v), .000000000000001), materialShininess);
    return ambient + diffuse + specular;
  }
  return ambient + diffuse;
}

void main()
{
  vec3 eyeNormal = normalize(normalMat * normal);
  vec4 eyePosition = mvMat * vec4(position,1.0);

  lightIntensity = phongModel(eyeNormal, eyePosition);

  gl_Position = mvpMat * vec4(position,1.0);
}

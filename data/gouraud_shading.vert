#version 330
in vec3 position;
in vec3 normal;

smooth out vec3 lightIntensity;

const vec4 lightPosition[2] = vec4[2](vec4(0,1,1,0), vec4(200,100,30,1));
const vec3 lightColor[2] = vec3[2](vec3(0.6), vec3(0.8));

uniform vec3 materialAmbient = vec3(.2,.2,.2);
uniform vec3 materialDiffuse = vec3(.7);
uniform vec3 materialSpecular = vec3(.2);
uniform float materialShininess = 2;

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

  vec3 diffuse = materialDiffuse * cos_sn;
  if (cos_sn > .0 && materialShininess > .0)
  {
    vec3 specular = materialSpecular * pow(max(dot(r,v), .000000000000001), materialShininess);
    return materialAmbient + diffuse + specular;
  }
  return materialAmbient + diffuse;
}

void main()
{
  vec3 eyeNormal = normalize(normalMat * normal);
  vec4 eyePosition = mvMat * vec4(position,1.0);

  lightIntensity = vec3(.0);
  for (int i = 0; i < 2; ++i)
  {
      lightIntensity += lightColor[i] * phongModel(lightPosition[i], eyeNormal, eyePosition);
  }

  gl_Position = mvpMat * vec4(position,1.0);
}

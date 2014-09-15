#version 330
in vec3 position;
in vec3 normal;

out vec3 lightIntensity;

uniform vec4 lightPosition = vec4(20,10,30,1);
uniform vec3 lightAmbiant = vec3(0.3);
uniform vec3 lightDiffuse = vec3(1.0);
uniform vec3 lightSpecular = vec3(0.1);

uniform vec3 materialAmbiant = vec3(.8,.8,.8);
uniform vec3 materialDiffuse = vec3(0);
uniform vec3 materialSpecular = vec3(1.0);
uniform float materialShininess = 5;

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 projectionMat;
uniform mat4 mvpMat;

void main()
{
  // Convert normal and position to eye coords
  vec4 eyeCoords = mvMat * vec4(position,1.0);

  vec3 n = normalize(normalMat * normal);
  vec3 s = normalize(vec3(lightPosition - eyeCoords));
  vec3 v = normalize(-eyeCoords.xyz);
  vec3 r = reflect(-s, n);

  float cos_sn = max(dot(s, n), 0.0);

  vec3 ambiant = lightAmbiant * materialAmbiant;
  vec3 diffuse = lightDiffuse * materialDiffuse * cos_sn;
  vec3 specular = vec3(.0);
  if (cos_sn > .0)
  {
    specular = lightSpecular * materialSpecular * pow(max(dot(r,v), .0), materialShininess);
  }

  lightIntensity = ambiant + diffuse + specular;

  // Convert position to clip coordinates and pass along
  gl_Position = mvpMat * vec4(position,1.0);
}
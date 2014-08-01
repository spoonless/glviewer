#version 420

smooth in vec3 normalFrag;
smooth in vec3 positionFrag;

out vec4 color;

void main()
{
  vec3 lightPosition = vec3(2, 2, 50);
  vec3 lightColor = vec3(1, 1, 1) - 0.02;
  vec3 ambientContrib = vec3(0.01, 0.0, 0.01);
  
  vec3 normal = normalize(normalFrag);

  vec3 lightVec = normalize(lightPosition - positionFrag);
  vec3 viewVec = normalize(-positionFrag);
  vec3 reflectVec = reflect(-lightVec, normal);
  
  float spec = max(dot(reflectVec, viewVec), 0.0);
  spec = pow(spec, 16.0);
  
  vec3 specContrib = lightColor * spec;
  vec3 diffContrib = lightColor * max(dot(lightVec, normal), 0.0);

  vec3 lightContribution = ambientContrib + diffContrib;
  color = vec4(lightContribution, 1.0);
}

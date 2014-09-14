#version 330

smooth in vec3 normalFrag;
smooth in vec3 positionFrag;

uniform vec3 color = vec3(1, 1, .8) - .2;

out vec4 frag_color;

void main()
{
  vec3 lightPosition = vec3(2, 2, 50);
  vec3 ambientContrib = vec3(0.01, 0.0, 0.01);
  
  vec3 normal = normalize(normalFrag);

  vec3 lightVec = normalize(lightPosition - positionFrag);
  vec3 viewVec = normalize(-positionFrag);
  vec3 reflectVec = reflect(-lightVec, normal);
  
  float spec = max(dot(reflectVec, viewVec), 0.0);
  spec = pow(spec, 16.0);
  
  vec3 specContrib = color * spec;
  vec3 diffContrib = color * max(dot(lightVec, normal), 0.0);

  vec3 lightContribution = ambientContrib + diffContrib;
  frag_color = vec4(lightContribution, 1.0);
}

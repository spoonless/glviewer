#version 330

/***************************************************/
/* vertex attributes                               */
/***************************************************/

in vec3 vertexPosition;
in vec3 vertexNormal;

/***************************************************/
/* out variables                                   */
/***************************************************/

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;

/***************************************************/
/* matrices                                        */
/***************************************************/

uniform mat3 normalMat;
uniform mat4 mvMat;
uniform mat4 mvpMat;

void main()
{
    fragNormal = normalize(normalMat * vertexNormal);
    fragPosition = (mvMat * vec4(vertexPosition,1.0)).xyz;
    gl_Position = mvpMat * vec4(vertexPosition,1.0);
}

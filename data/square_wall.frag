#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main( void ) {

	vec2 position = 10.0 * (( gl_FragCoord.xy / resolution.xy ) + mouse / 4.0);
	position.y *= resolution.y/resolution.x;
	position.x += step(0.5, fract(0.5 * position.y)) * 0.5;

	float color = 0.0;
	float v = step(0.1, fract(position.x)) - step(0.9, fract(position.x));
	float k = step(0.1, fract(position.y)) - step(0.9, fract(position.y));
	color += v*k;
	gl_FragColor = vec4( vec3(color), 0.2 );

}


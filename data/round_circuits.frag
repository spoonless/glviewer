// joko interactive

#ifdef GL_ES
precision mediump float;
#endif

#define TWO_PI 6.28318530717958646
#define PI 3.14159265358979323
#define HALF_PI 1.570796326794896615
#define TAU (PI*2.0)

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
varying vec2 surfacePosition;

//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.75;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.45;

float rand(vec2 n)
{
	return fract(sin(n.x*2732.7357+n.y*2542.3643)*4365.6247);	
}
void main( void )
{
	vec2 pos = surfacePosition;
    	//determine the vector length of the center position
    	float len = length(pos);
	pos = pos * .5 + .5;
	
	//float scale = 35.;
	float overallScale = 30.;
	vec2 scale = vec2(overallScale);
	vec2 spot = floor(pos * scale);
	float base = rand(spot);
	
	float check = abs(mod(spot.x, 2.) - mod(spot.y, 2.));
	float sq = 1.;//.85 + .15 * check;
	
	
	vec2 xp = scale * pos - spot;
	
	vec2 v1;
	vec2 v2;
	
	float rot = 0.;
	
	float reverse = 0.;
	if(base > .5)
	{
		v1 = xp;
		v2 = vec2(1., 1.) - xp;
	}
	else
	{
		rot = 1.;
		v1 = vec2(xp.x, 1. - xp.y);
		v2 = vec2(1. - xp.x, xp.y);
	}
	
	float ln1 = length(v1);
	float ln2 = length(v2);
	
	float xpf = 1.;
	float range = .05;
	float range2 = .08;
	float dim = .5;
	
	float alt = 0.;
	vec2 corner;
	//float r = min(abs(ln1 - .5), abs(ln2 - .5));
	float r1 = abs(ln1 - .5);
	float r2 = abs(ln2 - .5);
	float r = 2. * range2;
	if(r1 < range2)
	{
		r = r1;
		corner = v1;
	}
	else if(r2 < range2)
	{
		r = r2;
		corner = v2;
		alt = 1.;
	}
	
	float spark = 0.;
	
	if(r < range2)
	{
		if(r < range)
		{
			xpf = dim;
		}
		else
		{
			float a = (r - range)/(range2 - range);
			xpf = mix(dim, 1., a);
		}
		
		if(r < .03)
		{
			float ang = atan(corner.y, corner.x);
			
			if(check != 0.) ang = .5 * PI - ang;
			
			float amid = .5 * PI * mod(time, 1.);
			float arange2 = .09 * PI;
			
			if(mod(ang - amid,PI/2.) < arange2)
			{
				spark = 1.;
			}
		}
	}
	
	float timePerc = mod(time * .01, 2.);
	if(timePerc > 1.) timePerc = 2. - timePerc;
	
	vec3 lefta = vec3(1., .5, 1.);
	vec3 leftb = vec3(0., .5, 1.);
	vec3 left = mix(lefta, leftb, timePerc);
	
	float timePerc2 = mod(time * .05, 2.);
	if(timePerc2 > 1.) timePerc2 = 2. - timePerc2;
	
	vec3 righta = vec3(0., 0., 1.);
	vec3 rightb = vec3(1., 1., 0.);
	vec3 right = mix(righta, rightb, timePerc2);
	
	vec3 squareColor = sq * xpf * mix(left, right, pos.x) + spark;
	

    //use smoothstep to create a smooth vignette
    float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);
	
	
	gl_FragColor = vec4(
		mix(squareColor, squareColor * vignette, .3), 1.);
}


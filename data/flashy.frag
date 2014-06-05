//MG
//Playing with water effects sourced from:
//	'Water' by Viktor Korsun (BIT_TEK) (2012)
//Which can be found at http://www.iquilezles.org/apps/shadertoy/
//under "water" in the presets list
#ifdef GL_ES
precision mediump float;
#endif
#define PI		3.1415926535897932
#define SINUSOID_SPD	0.4
#define RATE            10.
//speed
const float speed = 6.2;
const float speed_x = 3.2;
const float speed_y = 2.3;
// geometry
const float intensity = 2.3;
const int steps = 64;
const float frequency = 1.0;
const int angle = 7; // better when a prime
// reflection and emboss
const float delta = 40.;
const float intence = 400.0;
const float emboss = 0.3;
uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void shader(vec2,float);
vec3 water_refract();
float col(vec2);


vec2 complex_mul(vec2 factorA, vec2 factorB){
   return vec2( factorA.x*factorB.x - factorA.y*factorB.y, factorA.x*factorB.y + factorA.y*factorB.x);
}

vec2 complex_div(vec2 numerator, vec2 denominator){
   return vec2( numerator.x*denominator.x + numerator.y*denominator.y,
                numerator.y*denominator.x - numerator.x*denominator.y)/
          vec2(denominator.x*denominator.x + denominator.y*denominator.y);
}

vec2 torus_mirror(vec2 uv){
	return vec2(1.)-abs(fract(uv*.5)*2.-1.);
}

void main( void ) {
	vec2 aspect = vec2(1.,resolution.y/resolution.x);
	vec2 uv = (gl_FragCoord.xy / resolution.xy - 0.5)*aspect;
	float mouseW = -atan((mouse.y - 0.5), (mouse.x - 0.5));
	vec2 mousePolar = vec2(-cos(mouseW), sin(mouseW));
	vec2 offset = 0.5 + complex_mul((mouse-0.5)*vec2(-1.,1.)*aspect, mousePolar)*aspect*8. ;
	
	vec2 pos = torus_mirror(complex_div(mousePolar*vec2(0.2), uv) - offset).yx;
	pos.x-=resolution.x/resolution.y/2.0;pos.y-=0.5;
	vec3 c1=water_refract();
	pos.xy+=c1.xy;
	
	shader(pos,c1[2]);
}

void shader(vec2 pos,float alpha) {
	//pos+=time*0.1;
	vec2 mse;
	mse.x+=(mouse.x-0.5)*resolution.x/resolution.y;mse.y+=mouse.y-0.5;
	
	float fx=sin(pos.x*10.0+time*SINUSOID_SPD)/4.0;
	float dist=abs(pos.y-fx)*80.0;
	gl_FragColor+=vec4(0.5/dist*alpha,0.5/dist*alpha,1.0/dist*alpha,1.0);
	
	fx=cos(pos.x*10.0+time*SINUSOID_SPD*2.5)/4.0;
	dist=abs(pos.y-fx)*80.0;
	gl_FragColor+=vec4(1.0/dist*alpha,0.5/dist*alpha,0.5/dist*alpha,1.0);
	
	
	if (pos.y<=-0.2) {
		gl_FragColor+=vec4(abs(pos.y+0.2)*alpha,abs(pos.y+0.2)*alpha,abs(pos.y+0.2)*alpha,1.0);
	}
	if(fract(time*RATE) > 0.5){
		gl_FragColor = vec4(1.0 - gl_FragColor.xyz, 1.0);
	}
}

vec3 water_refract() {
	vec2 p=(gl_FragCoord.xy/resolution.y);
	p.x-=resolution.x/resolution.y/2.0;p.y-=0.5;
	vec2 c1 = p, c2 = p;
	float cc1 = col(c1);
	
	c2.x += resolution.x/delta;
	float dx = emboss*(cc1-col(c2))/delta;
	
	c2.x = p.x;
	c2.y += resolution.y/delta;
	float dy = emboss*(cc1-col(c2))/delta;
	
	c1.x = dx;
	c1.y = -dy;
	
	float alpha = 1.+dot(dx,dy)*intence;
	
	return vec3(vec2(c1),alpha);
}

float col(vec2 coord)
{
	float delta_theta = 2.0 * PI / float(angle);
	float col = 0.0;
	float theta = 0.0;
	for (int i = 0; i < steps; i++)
	{
		vec2 adjc = coord;
		theta = delta_theta*float(i);
		adjc.x += cos(theta)*time*speed + time * speed_x;
		adjc.y -= sin(theta)*time*speed - time * speed_y;
		col = col + cos( (adjc.x*cos(theta) - adjc.y*sin(theta))*frequency)*intensity;
	}
	return cos(col);
}


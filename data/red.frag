// "Cobweb" by Kabuto 
// Based on @ahnqqq's blob raymarcher
// Little changes by anoki

// rotwang @mod* colors, @mod+ vignette
# ifdef GL_ES
precision mediump float;
# endif

uniform float time;
uniform vec2 resolution;

// colors
const vec3 viralDiffuse = vec3( .199, .165, 0.2 );
const vec3 viralEps = vec3( .001, 0., 0. );
const int viralIter = 128;
float viralSQ = sqrt(2.0)*0.5;

float cViral( vec3 p )
{
	vec3 q = abs(mod(p+vec3(cos(p.z*0.5), cos(p.x*0.5), cos(p.y*0.5)),2.6)-1.0);
	float a = q.x + q.y + q.z - min(min(q.x, q.y), q.z) - max(max(q.x, q.y), q.z);
	q = vec3(p.x+p.y, p.y+p.z, p.z+p.x)*viralSQ;
	q = abs(mod(q,2000.0)-1.0);
	float b = q.x + q.y + q.z - min(min(q.x, q.y), q.z) - max(max(q.x, q.y), q.z);
	return min(a,b);
}

vec3 nViral( vec3 p )
{
	float o = cViral( p );
	return normalize( o - vec3( cViral( p - viralEps ), cViral( p - viralEps.zxy ), cViral( p - viralEps.yzx ) ) );
}

void main()
{
	float aspect = resolution.x / resolution.y;
	vec2 p = gl_FragCoord.xy / resolution * 2. - 1.;
	vec2 m = vec2(0.5,-0.5);
	p.x *= aspect;
	m.x *= aspect;
	
	vec3 o = vec3( 0., 0., time );
	vec3 s = vec3( m, 0. );
	vec3 b = vec3( 0., 0., 0. );
	vec3 d = vec3( p, 1. ) / 32.;
	vec3 t = vec3( .5 );
	vec3 a;
	
	for( int i = 0; i < viralIter; ++i )
	{
		float h = cViral( b + s + o );
		//if( h < 0. )
		//	break;
		b += h * 10.0 * d;
		t += h;
	}
	t /= float( viralIter );
	a = nViral( b + s + o );
	float x = dot( a, t );
	t = ( t + pow( x, 4. ) ) * ( 1. - t * .01 ) * viralDiffuse;
	t *= b.z *0.125 ; 
	
	vec4 color = vec4( t*2.0, 1. );
	gl_FragColor = color;
	gl_FragColor.b = 0.0;
	gl_FragColor.g = 0.0;	
}


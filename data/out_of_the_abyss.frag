#ifdef GL_ES
precision mediump float;
#endif
uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

//JULIO NUÑEZ - ITMAZ - simple perlin noise

// Shabby - parallax frost mod
float rand3( float x, float y ){return fract( sin( x + y*0.1234 )*1234.0 );}

float interpolar(vec2 cord, float L){
   	float XcordEntreL= cord.x/L;
        float YcordEntreL= cord.y/L;
    
	float XcordEnt=floor(XcordEntreL);
        float YcordEnt=floor(YcordEntreL);

	float XcordFra=fract(XcordEntreL);
        float YcordFra=fract(YcordEntreL);
	
	float l1 = rand3(XcordEnt, YcordEnt);
	float l2 = rand3(XcordEnt+1.0, YcordEnt);
	float l3 = rand3(XcordEnt, YcordEnt+1.0);
	float l4 = rand3(XcordEnt+1.0, YcordEnt+1.0);
	
	float inter1 = (XcordFra*(l2-l1))+l1;
	float inter2 = (XcordFra*(l4-l3))+l3;
	float interT = (YcordFra*(inter2 -inter1))+inter1;
    return interT;
}

#define N 12
void main(void)
{	
	float color = 0.0;
	
	for ( int i = 0; i < N; i++ ){
		float p = fract(float(i) / float(N) - time*-.1 );
		float a = p * (0.90-p);
		color += a * (interpolar(gl_FragCoord.xy-resolution/2., resolution.y/pow(2.0, p*p*float(N)))-.5);
	}
	color += 0.5;
	gl_FragColor = vec4(0.5,0.5,0.5,1.0)*color*1.;
	
}


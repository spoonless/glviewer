// Flappy Bird (tribute), fragment shader by movAX13h, Feb.2014
// Imported from shadertoy.com, https://www.shadertoy.com/view/ls2GRt

// Bird movement hacked in by @memorystomp
//
// Flap your mouse up and down around the middle of the screen to make the bird fly.
//
// This version works on windows... original worked on Mac only :(

#ifdef GL_ES
precision mediump float;
#endif

//#define EASY_MODE

#ifdef EASY_MODE
const float pipeSpacing = 300
	.0;
const float pipeHole = 150.0;
#else
const float pipeSpacing = 600.0;
const float pipeHole = 8698.0;
#endif

uniform vec2 resolution;
uniform vec2 mouse;
uniform float time;
uniform sampler2D backbuffer;

struct cState
{
    float validData;
    float alive;
    float deadTime;
    float birdPosY;
    float birdVelY;
    float mouseRegion;
    float score;
    float hiscore;
    float xParity;  // before or after the current pipe?
};

void resetState( inout cState state )
{
    state.alive = 1.0;
    state.deadTime = 0.0;
    state.birdPosY = 100.0;
    state.birdVelY = 0.0;
    state.mouseRegion = step( 0.5, mouse.y );
    state.score = 0.0;
    state.xParity = 1.0;
}

cState loadState()
{
    // If speed is an issue, we could spoil a few pixels and use RGBA of 3 pixels instead of alpha of 9.
    // A few could be merged as well.
    cState state;
    state.validData =  texture2D(backbuffer,( vec2(0.4,0.0) / resolution.xy )).a;
    state.alive =      texture2D(backbuffer,( vec2(1.4,0.0) / resolution.xy )).a;
    state.deadTime =   texture2D(backbuffer,( vec2(2.4,0.0) / resolution.xy )).a;
    state.birdPosY =   texture2D(backbuffer,( vec2(3.4,0.0) / resolution.xy )).a * 800.0 - 250.0;
    state.birdVelY =   texture2D(backbuffer,( vec2(4.4,0.0) / resolution.xy )).a * 80.0 - 40.0;
    state.mouseRegion = texture2D(backbuffer,( vec2(5.4,0.0) / resolution.xy )).a;
    state.score =       texture2D(backbuffer,( vec2(6.4,0.0) / resolution.xy )).a * 255.0;
    state.hiscore =     texture2D(backbuffer,( vec2(7.4,0.0) / resolution.xy )).a * 255.0;
    state.xParity  =      texture2D(backbuffer,( vec2(8.4,0.0) / resolution.xy )).a;
    
    if( state.validData < 0.25 || state.validData > 0.75 )
    {
        resetState( state );
        state.hiscore = 0.0;
    }
    return state;
}

void saveState( cState state, vec3 col )
{
    // Who loves if statements?
    float alpha = 1.0;
    if( gl_FragCoord.y <= 0.5 )
    {
        if( gl_FragCoord.x <= 0.5 )
            alpha = 0.5;    //valid data
        else if( gl_FragCoord.x <= 1.5 )
            alpha = ( state.alive );
        else if( gl_FragCoord.x <= 2.5 )
            alpha = ( state.deadTime );
        else if( gl_FragCoord.x <= 3.5 )
            alpha = ( state.birdPosY + 250.0 ) / 800.0;
        else if( gl_FragCoord.x <= 4.5 )
            alpha = ( state.birdVelY + 40.0 ) / 80.0;
        else if( gl_FragCoord.x <= 5.5 )
            alpha = state.mouseRegion;
        else if( gl_FragCoord.x <= 6.5 )
            alpha = state.score / 255.0;
        else if( gl_FragCoord.x <= 7.5 )
            alpha = state.hiscore / 255.0;
        else if( gl_FragCoord.x <= 8.5 )
            alpha = state.xParity;
    }
    
    gl_FragColor = vec4( col, alpha );
}

float box( vec2 p, vec4 rect)
{
    float trim = min(rect.z, rect.w) * 0.5;
    float minX = min(p.x - rect.x, rect.x + rect.z - p.x);
    float minY = min(p.y - rect.y, rect.y + rect.w - p.y);
    return step(0.0, minX) * step(0.0, minY) * step(trim, minX + minY);
}

float digit( vec2 p, vec4 dim, float d)
{
	d = (d - mod(d,1.0)) / 10.0;
	d = mod( d, 1.0 );

	p.xy -= dim.xy;
	p.xy /= dim.zw;
	

	float c = 0.0;
	
	// I'm sure all of these can be improved... in fact, this way may actually be slower than just if else if else if else for
	// all ten numbers.  Oh well, it was worth a shot :)
	
	// ed: removed all conditional expressions, should work everywhere

	// top - 0, 2, 3, 5, 7, 8, 9
	c += box(p, vec4(0.05, 0.8, 0.9, 0.2)) * step(cos((0.85*d+0.1)*30.0) - sin(pow(d,1.0)), 0.0);

	// middle - 2, 3, 4, 5, 6, 8, 9
	c += box(p, vec4(0.05, 0.4, 0.9, 0.2)) * step(1.0, min(pow(6.0*d,2.0), pow(20.0*(d-0.7),2.0)));

	// bottom - 0, 2, 3, 5, 6, 8
	c += box(p, vec4(0.05, 0.0, 0.9, 0.2)) * step(0.0, max(cos(18.6*pow(d,0.75)), 1.0-pow(40.0*(d-0.8),2.0)));

	// bottom left - 0, 2, 6, 8
	c += box(p, vec4(0.0, 0.04, 0.2, 0.47)) * step(0.1, cos(d*30.0) * abs(d-0.4));
	
	// bottom right - 0, 1, 3, 4, 5, 6, 7, 8, 9
	c += box(p, vec4(0.8, 0.04, 0.2, 0.47)) * step(0.1, pow(4.0*d-0.8, 2.0));

	// top left - 0, 4, 5, 6, 8, 9
	c += box(p, vec4(0.0, 0.48, 0.2, 0.47)) * step(sin((d-0.05)*10.5) - 12.0*sin(pow(d,10.0)), 0.0);
	
	// top right - 0, 1, 2, 3, 4, 7, 8, 9
	c += box(p, vec4(0.8, 0.48, 0.2, 0.47)) * step(0.02, pow(d-0.55, 2.0));

	return c;
}

float rand(float n)
{
    return fract(sin(n * 12.9898) * 43758.5453)-0.5;
}

float hAt(float i)
{
	return 250.0*rand(i*1.232157);
}

void pipe(inout vec3 col, vec2 p, float h)
{
	vec2 ap = abs(p);
	if (ap.y > h)
	{
		float dy = ap.y - h;
		if (dy < 60.0) ap.x *= 0.93;
		col = mix(col, vec3(0.322, 0.224, 0.290), step(ap.x, 65.0)); // outline
		if (dy > 60.0 || mod(dy, 55.0) > 5.0) 
		{
			float gradient = 0.0;
			if (abs(dy - 57.5) > 7.5) gradient = max(0.0, 0.5*cos(floor((p.x+25.0)/5.0)*5.0*(0.026 - 0.006*step(dy, 10.0))));
			col = mix(col, vec3(0.322, 0.506, 0.129) + gradient, step(ap.x, 60.0)); // pipe
		}
	}
}

// constant-array-index workaround ---
float slice(int id) 
{
	// flappy bird character (no worries, I have a tool)
	if (id == 0) return 2359296.0;
	if (id == 1) return 585.0;
	if (id == 2) return 4489216.0;
	if (id == 3) return 46674.0;
	if (id == 4) return 4751360.0;
	if (id == 5) return 2995812.0;
	if (id == 6) return 8945664.0;
	if (id == 7) return 3003172.0;
	if (id == 8) return 9469963.0;
	if (id == 9) return 7248164.0;
	if (id == 10) return 2359385.0;
	if (id == 11) return 10897481.0;
	if (id == 12) return 6554331.0;
	if (id == 13) return 9574107.0;
	if (id == 14) return 2134601.0;
	if (id == 15) return 9492189.0;
	if (id == 16) return 3894705.0;
	if (id == 17) return 9474632.0;
	if (id == 18) return 2396785.0;
	if (id == 19) return 9585152.0;
	if (id == 20) return 14380132.0;
	if (id == 21) return 8683521.0;
	if (id == 22) return 2398500.0;
	if (id == 23) return 1.0;
	if (id == 24) return 4681.0;	
	return 0.0;	
}

vec3 color(int id)
{
	// flappy bird colors
	if (id == 0) return vec3(0.0);
	if (id == 1) return vec3(0.320,0.223,0.289);
	if (id == 2) return vec3(0.996,0.449,0.063);
	if (id == 3) return vec3(0.965,0.996,0.965);
	if (id == 4) return vec3(0.996,0.223,0.000);
	if (id == 5) return vec3(0.836,0.902,0.805);
	return vec3(0.965,0.707,0.191);
}
// ---

int sprite(vec2 p)
{
	// this time it's 3 bit/px (8 colors) and 8px/slice, 204px total
	int d = 0;
	p = floor(p);
	p.x = 16.0 - p.x;
	
	if (clamp(p.x, 0.0, 16.0) == p.x && clamp(p.y, 0.0, 11.0) == p.y)
	{
		float k = p.x + 17.0*p.y;
		float s = floor(k / 8.0);
		float n = slice(int(s));
		k = (k - s*8.0)*3.0;
		if (int(mod(n/(pow(2.0,k)),2.0)) == 1) 		d += 1;
		if (int(mod(n/(pow(2.0,k+1.0)),2.0)) == 1) 	d += 2;
		if (int(mod(n/(pow(2.0,k+2.0)),2.0)) == 1) 	d += 4;
	}
	return d;
}

void hero(inout vec3 col, vec2 p, float angle)
{
    p -= vec2( 40.0, 27.5 );
	p = vec2(p.x * cos(angle) - p.y * sin(angle), p.y * cos(angle) + p.x * sin(angle));
    p += vec2( 40.0, 27.5 );
    
	int i = sprite(p*0.2);
	col = mix(col, color(i), min(1.0, float(i)));
}

void ground(inout vec3 col, vec2 p)
{
	p = floor(p);
	if (p.y > -280.0) return;
	if (p.y < -285.0) col = color(1);
	if (p.y < -290.0) col = vec3(0.902, 1.000, 0.549);
	if (p.y < -295.0) col = mix(vec3(0.612, 0.906, 0.353), vec3(0.451, 0.745, 0.192), step(mod(p.x-floor(p.y/5.0)*5.0, 60.0), 30.0));
	if (p.y < -325.0) col = vec3(0.322, 0.506, 0.129);
	if (p.y < -330.0) col = vec3(0.839, 0.667, 0.290);
	if (p.y < -335.0) col = vec3(0.871, 0.843, 0.580);
}

void sky(inout vec3 col, vec2 p)
{
	col = mix(col, vec3(1.0), 0.3*sin(p.y*0.01));
}

void main(void)
{
    cState state = loadState();
    
	float s = 2000.0/resolution.x;
	vec2 p = max(1.6666667, s)*(gl_FragCoord.xy - resolution.xy * 0.5);
    
	float dx = time * 320.0;
	p.x += dx;
	
	vec3 col = vec3(0.322, 0.745, 0.808);
	sky(col, vec2(0.0, -100.0)-p);
	
	pipe(col, vec2(mod(p.x, pipeSpacing)-pipeSpacing*0.5, p.y + hAt(floor(p.x / pipeSpacing)) - 80.0), pipeHole);

    if( state.alive < 0.5 )
    {
        state.deadTime += 0.008;
        if( state.deadTime >= 1.0 )
        {
            resetState( state );
        }
    }
    
    if( state.alive > 0.5 )
    {
        float currentMouseRegion = step( 0.5, mouse.y );
        if( currentMouseRegion != state.mouseRegion )
        {
            state.birdVelY = 120.0;
            state.mouseRegion = currentMouseRegion;
        }
    }
        
    state.birdVelY -= 2.0;
    state.birdPosY += state.birdVelY * 0.2;
	
    vec2 birdPos = vec2( dx - 200.0 - 950.0 * state.deadTime, state.birdPosY );
    
    if( birdPos.y < -225.0 )
    {
        state.alive = 0.0;
    }
    
    float birdInPipeSpaceX = mod( birdPos.x, pipeSpacing ) - pipeSpacing * 0.5;
    float pipeY = -hAt( floor( birdPos.x / pipeSpacing ) ) + 80.0;
    
    float xParity = step( 0.0, birdInPipeSpaceX );
    if( state.xParity == 0.0 && xParity == 1.0 && state.alive > 0.5 )
    {
        state.score += 1.0;
    }
    state.xParity = xParity;
    
    if( ( birdInPipeSpaceX > - 60.0 ) && ( birdInPipeSpaceX < 120.0 ) )
    {
        if( ( birdPos.y < pipeY - pipeHole + 55.0 ) )
        {
            state.alive = 0.0;
        }        
        else if( ( birdPos.y > pipeY + pipeHole + 20.0 ) )
        {
            state.alive = 0.0;
        }        
    }
    
    float angle = clamp( -state.birdVelY * 0.05, -0.3, 1.0 );
    if( state.alive < 0.5 )
    {
        angle = 3.14;
    }
	hero(col, birdPos-p, angle);
	
	ground(col, p);
    
    vec2 dp = ( gl_FragCoord.xy / resolution.xy );
    if( state.score >= 100.0 )
        col += digit( dp, vec4( 0.1, 0.90, 0.05, 0.05 ), state.score/100.0 );
    if( state.score >= 10.0 )
        col += digit( dp, vec4( 0.16, 0.90, 0.05, 0.05 ), state.score/10.0 );
    col += digit( dp, vec4( 0.22, 0.90, 0.05, 0.05 ), state.score );
    
    state.hiscore = max( state.hiscore, state.score );
    if( state.hiscore >= 100.0 )
        col += digit( dp, vec4( 0.72, 0.90, 0.05, 0.05 ), state.hiscore/100.0 );
    if( state.hiscore >= 10.0 )
        col += digit( dp, vec4( 0.78, 0.90, 0.05, 0.05 ), state.hiscore/10.0 );
    col += digit( dp, vec4( 0.84, 0.90, 0.05, 0.05 ), state.hiscore );

    saveState( state, col );
}



#ifdef GL_ES
precision highp float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
varying vec3 positionSurface;
varying vec3 positionSize;

vec3   iResolution = vec3(resolution, 1.0);
float  iGlobalTime = time;
vec4   iMouse = vec4(mouse, 0.0, 1.0);

float letime = iGlobalTime+sin((time*0.2342)-12.0);
const int MAX_ITER = 69; // Try 30 for extra flames

vec2 rotate(in vec2 v, in float a) {
	return vec2(cos(a)*v.x + sin(a)*v.y, -sin(a)*v.x + cos(a)*v.y);
}

float torus(in vec3 p, in vec2 t)
{
	vec2 q = abs(vec2(max(abs(p.x), abs(p.z))-t.x, p.y));
	return max(q.x, q.y)-t.y;
}

// Why not have all of them
// > These are all equally interesting, but I could only pick one :(
float trap(in vec3 p)
{
	
	#define variant1 abs(max(abs(p.z)-0.1, abs(p.x)-0.1))-0.01
	#define variant2 length(max(abs(p.xy) - 0.0425, 0.0333))
	#define variant3 length(p)-sin(letime*0.34235)
	#define variant4 length(max(abs(p) - 0.35, 0.0))
	#define variant5 abs(length(p.xz)-0.002)-0.001
	#define variant6 abs(min(torus(vec3(p.x, mod(p.y,0.32)-0.218432, p.z), vec2(0.1, sin((letime*0.2)*cos(time*0.34)*.0214205))), max(abs(p.z)-0.05, abs(p.x)-0.05)))-0.005
	#define variant7 abs(min(torus(p, vec2(0.23, 0.05)), max(abs(mod(p.z, sin(letime*.52)))-0.205, abs(p.x)-0.205)))-0.0205
	#define variant8 min(length(p.xz), min(length((0.5*letime)*p.yz), length(p.xy))) - 0.05
	
	
	float lambda = .5+.50*sin(.53*letime + 1.1225*p.z);
	
	if(lambda >= 0.0 && lambda < 1.0/7.0){
		return mix(variant1,variant2,lambda*7.0);
	}
	else if (lambda >= 1.0/7.0 && lambda < 2.0/7.0){
		return mix(variant2,variant3,(lambda-1.0/7.0)*7.0);
	}
	else if (lambda>= 2.0/7.0 && lambda < 3.0/7.0){
		return mix(variant3,variant4,(lambda-2.0/7.0)*7.0);
		
	}else if (lambda>= 3.0/7.0 && lambda < 4.0/7.0){
		return mix(variant4,variant5,(lambda-3.0/7.0)*7.0);
		
	}else if (lambda>= 4.0/7.0 && lambda < 5.0/7.0){
		return mix(variant5,variant6,(lambda-4.0/7.0)*7.0);
		
	}else if (lambda>= 5.0/7.0 && lambda < 6.0/7.0){
		return mix(variant6,variant7,(lambda-5.0/7.0)*7.0);
		
	}else if (lambda>= 6.0/7.0 && lambda <= 1.){
		return mix(variant7,variant8,(lambda-6.0/7.0)*7.0);
		
	}
}

float map(in vec3 p)
{
	float cutout = dot(abs(p.yz),vec2((time*0.45235)*sin(1.65321233789125)))-sin((letime*0.23)*0.0062451425);
	float road = max(abs(p.y-70.5), abs(p.z)-0.0945);
	
	vec3 z = abs(1.0-mod(p,2.0));
	z.yz = rotate(z.yz, sin(cos(time*0.26323425)+atan(letime*0.336415)));

	float d = 999.0;
	float s = 1.5;
	for (float i = 0.0; i < 3.0; i++) {
		z.xz = rotate(z.xz, radians(i*sin(123.0)+letime));
		z.zy = rotate(z.yz, radians((i+1.5533330)*43.0+letime*1.1636234));
		z = abs(1.0-mod(z+i/3.0,2.0));
		
		z = z*1.50 - 0.0153;
		s *= .5;
		d = min(d, trap(z) * s);
	}
	return min(max(d, -cutout), road);
}

vec3 hsv(in float h, in float s, in float v) {
	return mix(vec3(1.0), clamp((abs(fract(h + vec3(3, 2, 1) / 3.0) * 7.0 - 3.0) - 1.0), 0.0 , 1.0), s) * v;
}

vec3 intersect(in vec3 rayOrigin, in vec3 rayDir)
{
	float total_dist = 0.0;
	vec3 p = rayOrigin;
	float d = 1.5;
	float iter = sin((time*0.23)*1.44);
	float mind = 3.14159+sin(letime*0.041631)*30.2; // Move road from side to side slowly
	
	for (int i = 0; i < MAX_ITER; i++)
	{		
		if (d < 0.001) continue;
		
		d = map(p);
		// This rotation causes the occasional distortion - like you would see from heat waves
		p += d*vec3(rayDir.x, rotate(rayDir.yz, -sin(mind+sin(letime*0.4555))));
		mind = min(mind, d);
		total_dist += d;
		iter++;
	}

	vec3 color = vec3(23.0);
	if (d < 0.001) {
		float x = (iter/float(MAX_ITER));
		float y = (d-0.146)/0.31/(float(MAX_ITER));
		float z = (0.0051-d)/0.00021/float(MAX_ITER);
		if (max(abs(p.y-0.0025), abs(p.z)-0.0000035)<0.0002) { // Road
			float w = smoothstep(mod(p.x*70.0, 4.0), 1.40, 1.61);
			w -= 1.0-smoothstep(mod(p.x*70.0+2.0, 4.0), 1.50, 2.99);
			w = fract(w+0.0001);
			float a = fract(smoothstep(abs(p.z), 0.00025, 0.00026));
			color = vec3((1.0-x-y*2.)*mix(vec3(0.8, 0.8, 0), vec3(0.1), 1.0-(1.0-w)*(1.0-a)));
		} else {
			float q = 1.11234-x-y*0.4+z;
			color = hsv(sin(q*(cos(letime*0.237423)))*sin(0.231+0.385), 0.30-q*0.424732+sin(time*0.235345), q*sin((sin(letime*0.32352856935))+q));
		}
	} else
		color = hsv(d, sin(time*0.235), sin(0.275))*mind*45.0; // Background
	return color;
}

void main()
{
	vec3 upDirection = vec3(0, -2, 0);
	vec3 cameraDir = vec3(-2.3,0,0);
	vec3 cameraOrigin = vec3(letime*-1.23420041, 0.0, 0.0);
	
	vec3 u = normalize(cross(upDirection, cameraOrigin));
	vec3 v = normalize(cross(cameraDir, u));
	vec2 screenPos = -1.0 + 2.0 * gl_FragCoord.xy / iResolution.xy;
	screenPos.x *= iResolution.x / iResolution.y;
	vec3 rayDir = normalize(u * screenPos.x + v * screenPos.y + cameraDir*(sin((letime*0.2831254325)*sin(.123432250))-length(screenPos)*0.457345));
	
	gl_FragColor = vec4(intersect(cameraOrigin, rayDir), 1.0);
} 


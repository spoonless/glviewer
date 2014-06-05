#ifdef GL_ES
precision highp float;
#endif

uniform float time;
uniform vec2 resolution;
varying vec2 surfacePosition;

float makePoint(float x,float y,float fx,float fy,float sx,float sy,float t){
   float xx=x+sin(.331*t*fx)*sx*0.7;
   float yy=y+cos(.332*t*fy)*sy*0.7;
   return (1.0+0.3*sin(t*fx+xx+yy))/sqrt(xx*xx+yy*yy);
}

void main( void ) {

   //vec2 p=(gl_FragCoord.xy/resolution.x)*2.0-vec2(1.0,resolution.y/resolution.x);
   vec2 p = surfacePosition;
   
   float time = time + time*time*0.2 + length(p)*2.*cos(length(p)*10.);
   
   p=p*0.9;
   
   float x=p.x;
   float y=p.y;

   float a;
   a=a+makePoint(x,y,3.3,2.9,0.3,0.3,time);
   a=a+makePoint(x,y,1.9,2.0,0.4,0.4,time);
   a=a+makePoint(x,y,0.8,0.7,0.4,0.5,time);
	/*
   a=a+makePoint(x,y,2.3,0.1,0.6,0.3,time);
   a=a+makePoint(x,y,0.8,1.7,0.5,0.4,time);
   a=a+makePoint(x,y,0.3,1.0,0.4,0.4,time);
   a=a+makePoint(x,y,1.4,1.7,0.4,0.5,time);
   a=a+makePoint(x,y,1.3,2.1,0.6,0.3,time);
   a=a+makePoint(x,y,1.8,1.7,0.5,0.4,time);   
   a=a+makePoint(x,y,1.2,1.9,0.3,0.3,time);
   a=a+makePoint(x,y,0.7,2.7,0.4,0.4,time);
   a=a+makePoint(x,y,1.4,0.6,0.4,0.5,time);
   a=a+makePoint(x,y,2.6,0.4,0.6,0.3,time);
   a=a+makePoint(x,y,0.7,1.4,0.5,0.4,time);
   a=a+makePoint(x,y,0.7,1.7,0.4,0.4,time);
   a=a+makePoint(x,y,0.8,0.5,0.4,0.5,time);
   a=a+makePoint(x,y,1.4,0.9,0.6,0.3,time);
   a=a+makePoint(x,y,0.7,1.3,0.5,0.4,time);
   a=a+makePoint(x,y,3.7,0.3,0.3,0.3,time);
   a=a+makePoint(x,y,1.9,1.3,0.4,0.4,time);
   a=a+makePoint(x,y,0.8,0.9,0.4,0.5,time);
   a=a+makePoint(x,y,1.2,1.7,0.6,0.3,time);
   a=a+makePoint(x,y,0.3,0.6,0.5,0.4,time);
   a=a+makePoint(x,y,0.3,0.3,0.4,0.4,time);
   a=a+makePoint(x,y,1.4,0.8,0.4,0.5,time);
   a=a+makePoint(x,y,0.2,0.6,0.6,0.3,time);
   a=a+makePoint(x,y,1.3,0.5,0.5,0.4,time);
   */
   vec3 a1=vec3(a*.02,a*.015,a*(.028+0.01*cos(time)));
 
   gl_FragColor = vec4(a1.x,a1.y,a1.z,1.0);
}


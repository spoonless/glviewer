#ifdef GL_ES
precision mediump float;
#endif

varying vec2 surfacePosition;
uniform float time;
uniform vec2 mouse;


const float color_intensity = .5;
const float Pi = 3.14159;

void main()
{
  vec2 p=(1.32*surfacePosition);
  for(int i=1;i<5;i++)
  {
    vec2 newp=p;
    newp.x+=.912/float(i)*sin(float(i)*Pi*p.y+time*0.15)+0.91;
    newp.y+=.913/float(i)*cos(float(i)*Pi*p.x+time*-0.14)-0.91;
    newp.y+=sin(mouse.x+cos(mouse.y));
    newp.x+=mouse.x*mouse.y*sin(time);
    p=newp;
  }
  vec3 col=vec3(sin(mouse.y+p.x+p.y)*.91+.1,sin(mouse.x+p.x+p.y)*.91+.1,sin(mouse.x+mouse.y+p.x+p.y)*.91+.1);
  gl_FragColor=vec4(col, 1.0);
}


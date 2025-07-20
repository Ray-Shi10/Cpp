#define EPS .01

vec2 normCoord(vec2 p) { return (p - iResolution.xy*.5) / min(iResolution.x, iResolution.y) * 2.2; }
bool keyPressed(int key) { return texture(iChannel0,vec2(float(key)/256.,0)).x>.0; }

float hash1(vec2 p) { return fract(sin(dot(p.xyx+.3,vec3(11.45,14.38,01.29)))*1e5); }
vec2 hash2(vec2 pos){ return 2.*fract(sin(vec2(dot(pos, vec2(432., 235.)), dot(pos, vec2(232., 735.)))) * 1000.) - 1.; }

float smoothMin(float a, float b, float k) { float h = clamp(.5+.5*(b-a)/k, 0., 1.); return mix( b, a, h ) - k*h*(1.-h); }

struct Ray { vec3 o, d; /**vec3 c;/**/ };
struct Camera { vec3 pos, dir, up; };

float pointDist(vec2 p) { return length(p); }
float rectDist(vec2 p, vec2 s) { vec2 d = abs(p)-s; return max(max(d.x,d.y), length(max(d,0.))); }
float lineDist(vec2 p, vec2 d) { return length(vec2(max(abs(dot(p,d)),dot(d,d))-dot(d,d),dot(p,vec2(-d.y,d.x))))/length(d); }

float noise1(vec2 a){vec2 i=floor(a),f=fract(a),u=f*f*(3.-2.*f);return mix(mix(dot(hash2(i),f),dot(hash2(i+vec2(1,0)),f-vec2(1,0)),u.x),mix(dot(hash2(i+vec2(0,1)),f-vec2(0,1)),dot(hash2(i+vec2(1)),f-vec2(1)),u.x),u.y);}

/*normal, smooth*/
vec2 senceDist(vec2 p) {
    float normalDist = 1e9;
    float smoothDist = 1e9;
    float dist1 = lineDist(p-vec2(.2,.0), vec2(.5,.0)) - .1; normalDist=min(normalDist, dist1); smoothDist=smoothMin(smoothDist, dist1, .2);
    float dist2 = rectDist(p, vec2(.1,.4)) - .1; normalDist=min(normalDist, dist2); smoothDist=smoothMin(smoothDist, dist2, .2);
    return vec2(normalDist, smoothDist);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = normCoord(fragCoord); if(/**max(abs(uv.x),abs(uv.y))>=1./*/abs(uv.x)>=iResolution.x/min(iResolution.x,iResolution.y)||abs(uv.y)>=iResolution.y/min(iResolution.x,iResolution.y)/**/) { fragColor = vec4(vec3(noise1(uv*(50.+noise1(uv)*20.)+iTime*01.)+1.)*.07+.75,1); return; }
    if(keyPressed(65)) return;
    //fragColor = vec4(vec3(hash1(uv)),1); return;
    //fragColor = vec4(vec3(noise1(uv*10.)+1.)*.5,1); return;
    vec2 sd = abs(senceDist(uv));
    if(sd.x <= EPS) fragColor = vec4(0,0,1,1);
    if(sd.y <= EPS) fragColor = vec4(1,1,0,1);
    vec2 mouse = normCoord(iMouse.xy);
    if(abs(abs(senceDist(mouse).y)-length(uv-mouse)) <= EPS) fragColor=vec4(0,0,0,1);
}

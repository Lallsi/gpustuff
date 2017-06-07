#version 450 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D img;

uniform mat4 C;
uniform float time;
uniform uint frame;
uniform int samplesPerFrame;

struct Material{
	vec3 color;
	bool refl;
};
struct Ray{
	vec3 o;
	vec3 d;
};
struct Hit{
	float t;
	vec3 n;
	Material m;
};


float noise3D(vec3 p)
{
	return fract(sin(dot(p ,vec3(12.9898,78.233,126.7378))) * 43758.5453)-0.5;
}

float noise2D(vec2 p){
    return fract(sin(dot(p,vec2(12.9898,78.233))) * 43758.5453)-0.5;
}

//  https://www.shadertoy.com/view/XsX3zB
//  Nikita Miropolskiy
/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}
vec2 random2(vec2 c) {
	float j = 4096.0*sin(dot(c,vec2(17.0, 59.4)));
	vec2 r;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}


//https://www.shadertoy.com/view/MsdGzl
// Inigo Quilez
//NOTE: Removed original comments
float hash(float seed)
{
    return fract(sin(seed)*43758.5453 );
}

vec3 cosineDirection( in float seed, in vec3 nor)
{
    vec3 tc = vec3( 1.0001+nor.z-nor.xy*nor.xy, -nor.x*nor.y)/(1.0001+nor.z); //NOTE: changed 1.0+nor.z to 1.0001+nor.z, otherwise if nor.z happened to be -1 we would get division by zero
    vec3 uu = vec3( tc.x, tc.z, -nor.x );										//the result seems ok )
    vec3 vv = vec3( tc.z, tc.y, -nor.y );
    
    float u = hash( 78.233 + seed);
    float v = hash( 10.873 + seed);
    float a = 6.283185 * v;

    return  sqrt(u)*(cos(a)*uu + sin(a)*vv) + sqrt(1.0-u)*nor;
}

bool planeIntersect(const Ray r, inout Hit h, vec3 n, float offset, Material m)
{
 	float d = dot(n*offset-r.o, n) / dot(r.d, n);
	if( d > 0.0 && d < h.t)
	{
		h.t = d;
		h.n = n;
		h.m = m;
		return true;
	}
	return false;
}

bool sphIntersect(const Ray r, inout Hit h, vec3 c, float radius, Material m){
	float d = pow(abs(dot(r.d,r.o-c)),2.0) - pow(abs(length(r.o-c)),2.0) + radius;
    if(d>0.0)
    {
        float b = -dot(r.d,(r.o-c));
        float d1 = b-sqrt(d);
        float d2 = b+sqrt(d);
        d = d1<d2 ? (d1<0.0 ? d2 : d1) : (d2<0.0 ? d1 : d2);
		if( d < 0.0)
			return false;
		if(d < h.t){
			h.t = d;
			h.n = normalize((r.o+r.d*d)-c);
			h.m = m;
			return true;
		}
    }

    return false;
}

bool intersect(const Ray r, out Hit h){
	
	h.t = 1.0 / 0.0;

	Material mSph1;
	mSph1.color = vec3(1.0);
	mSph1.refl = false;
	sphIntersect(r,h,vec3(-1.0,0.0,0.0),1.0, mSph1);
	
	Material mSph2;
	mSph2.color = vec3(1.0);
	mSph2.refl = true;
	sphIntersect(r,h,vec3(1.0,0.0,1.0),1.0, mSph2);
		
	Material mPlane1;
	mPlane1.color = vec3(1.0);
	mPlane1.refl = false;
	planeIntersect(r,h,vec3(0.0,1.0,0.0),-1.0, mPlane1);

	Material mPlane2;
	mPlane2.color = vec3(1.0);
	mPlane2.refl = false;
	planeIntersect(r,h,vec3(0.0,-1.0,0.0),-5.0, mPlane2);

	Material mPlane3;
	mPlane3.color = vec3(1.0,0.0,0.0);
	mPlane3.refl = false;
	planeIntersect(r,h,vec3(1.0,0.0,0.0),-3.0, mPlane3);
		
	Material mPlane4;
	mPlane4.color = vec3(0.0,1.0,0.0);
	mPlane4.refl = false;
	planeIntersect(r,h,vec3(-1.0,0.0,0.0),-3.0, mPlane4);
	
	Material mPlane5;
	mPlane5.color = vec3(1.0);
	mPlane5.refl = false;
	planeIntersect(r,h,vec3(0.0,0.0,1.0),-6.0, mPlane5);

	Material mPlane6;
	mPlane6.color = vec3(1.0);
	mPlane6.refl = false;
	planeIntersect(r,h,vec3(0.0,0.0,-1.0),-3.0, mPlane6);
	
	if( h.t != 1.0 / 0.0 )
		return true;
	return false;
}

Ray generateRay( vec2 p ){
	
	float fov_angle = radians(90.0);
	vec3 center = C[3].xyz;
	vec3 direction = C[2].xyz;
	vec3 up = C[1].xyz;
	vec3 horizontal = C[0].xyz;

	vec3 ro = center;
	vec3 rd = normalize( ( direction / tan( fov_angle*0.5 ) ) + p.x * horizontal + p.y * up );
	return Ray(ro,rd);
};

bool traceRay(inout Ray r, out Hit h, inout vec3 col, int depth) {

	if( intersect(r,h) )
	{
		
		r.o = r.o+r.d*h.t;
		if(h.m.refl)
			r.d = reflect(r.d,h.n);
		else
			r.d = cosineDirection(noise3D(vec3(gl_GlobalInvocationID.xy,time)+0.5)*12.97+float(depth)*7918.1818+float(frame)*76.22, h.n);

		//random point light to make area light
		vec2 rand = vec2(noise2D(vec2(gl_GlobalInvocationID.xy)*12.525+time), noise2D(vec2(gl_GlobalInvocationID.xy)*76.22+time));
		vec3 lp = vec3(rand.x,4.0,rand.y);
		vec3 ld = normalize(lp-r.o);
		Ray sr;
		sr.o = r.o+ld*0.0001f;
		sr.d = ld;
		Hit sh;
		sh.t = 1.0/0.0;
		bool shadow = intersect(sr,sh);
		if(!shadow||sh.t>length(lp-r.o))
			col += h.m.color*clamp(dot(h.n,r.d),0.0,1.0)*clamp(dot(h.n,ld),0.0,1.0);

		r.o += r.d*0.0001f;
		return true;
	}
	return false;
}

void main() {

	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec3 totalCol = vec3(0.0);
	for(int j = 0; j < samplesPerFrame; j++){
		vec2 offset = random3(vec3(vec2(pixelCoords),time+j*12.345)).xy+0.5;
		vec2 sampledCoords = vec2(pixelCoords) + offset;
		vec2 resolution = imageSize(img);
		vec2 uv = sampledCoords / resolution;
		vec2 p = 2.0*uv-1.0;
		p.x *= resolution.x/resolution.y;
	
		vec3 col = vec3(0.0);
	
		Ray r = generateRay(p);	
		Hit h;

		for(int i = 0; i < 3; i++)
		{
			if(!traceRay(r,h,col,i+j*10))
				break;
		
		}
		
		col = clamp(col, 0.0, 1.0);
		totalCol += col;
	}
	totalCol /= samplesPerFrame;

	vec4 pixelColor = vec4(totalCol, 1.0);
	vec4 previousColor = imageLoad(img, pixelCoords);
	vec4 resultColor = pixelColor / float(frame) + previousColor*(1.0 - 1.0 / float(frame));
	if(frame == 1)//reset image when camera has moved
		resultColor = pixelColor;
	imageStore(img, pixelCoords, resultColor);
}
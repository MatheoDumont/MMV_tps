// Texturing
// Eric Galin
#include "common.glsl"

const float view = 5000.0;  // View distance
const float Epsilon = 0.01; // Marching epsilon

// Iterations
const int Steps = 256;

// Lipschitz constant
const float K=7.0;

// (1) Ridge noise et tester differents terrains avec differentes fonctions : 2 * fabs(n)
// (2) Jouer sur parametres ampli / frequence
// (3) Reflechir 2 choses :
//      - Je veux pouvoir controler localement mon terrain
//      - Trouver des fonctions pour changment localement le relief d'elevation
//          Comment ? Que faire ? Ou agir ? Comment introduire les modifs locale => un lac par exemple
// (4) Le modele permet il de faire des terrains 3D ? Comment et ou faut il intervenir ? (grotte par exemple)
// cf Genevaux 2015
// (5) Comment texturer le modele en fonction de la geometrie -> neige en altitude tant que la pente le permet.


// Terrain heightfield
// x : Position in the plane
vec2 Terrain( in vec2 x )
{
	// Rotation matrix
	const mat2 m2 = mat2(0.8,-0.6,0.6,0.8);

    const float l0=1350.0; // freq
    const float a0=615.0; // amplitude

    
	vec2  p = x/l0;
    float a = 0.0;
    float b = 1.0;

    for( int i=0; i<3; i++ )
    {
        float n = 1.0 - abs(0.5 - Noise(p));  // ridge noise
        a += b*n;   
		b *= 0.5;
        p = m2*p*2.0;
    }

    for( int i=0; i<5; i++ )
    {
        float n = Noise(p); 
        a += b*n;
		b *= 0.5;
        p = m2*p*2.0;
    }
	return vec2(-400.0+a0*a, 1.0);
}

vec2 Eau(in vec2 x)
{
    return vec2(425.0 + 5.0 * sin(x.x / 20.0), 1.0);
}

vec2 disque(in vec2 x, in vec3 centre, in float radius)
{   
    vec2 dir = x - centre.xy;
    float alpha = dir.x * dir.x + dir.y * dir.y;
    float radius2 = radius * radius;
    if (alpha < radius2)
    {
        // g(x)
        alpha = (1.0 - (alpha / radius2));
        alpha = alpha * alpha * alpha;
    }
    else
        alpha = 0.;

    float height = centre.z;
    return vec2(height, alpha);
}

vec2 blend(in vec2 a, in vec2 b)
{
    float new_alpha = (a.y + b.y);
    float new_height = (a.x*a.y + b.x*b.y) / new_alpha;
    return vec2(new_height, new_alpha);
}

vec2 replace(in vec2 a, in vec2 b)
{
    float h = (1. - b.y) * a.x + b.x*b.y;
    return vec2(h, a.y);
}

vec2 comp_height(in vec3 p, in vec2 output_func_height)
{
    return vec2(p.z - output_func_height.x, output_func_height.y);
}

// Pour utiliser des surfaces implicites
float Union(in float a, in float b)
{
    return min(a, b);
}

float Inter(in float a, in float b)
{
    return max(a, b);
}

float Diff(in float a, in float b)
{
	return max(a, -b);
}

/**
 * Permet de blend des surfaces implicites.
 * 
 * Le coefficient k permet de dire à quel point on veut une jointure lisse
 * ex: - k = 1.0, équivalent à l'Union
 *     - k = 100.0, jointure relativement lisse
 */
float BlendImplicite(in float a, in float b, in float k)
{
    float h = max(k - abs(a - b), 0.f) / k;
    return min(a, b) - h * h * h * k * (1.f / 6.f);
}

float Sphere(in vec3 p, in vec3 center, in float radius, in float noise)
{
    return length(p - center) - radius + noise;
}


vec2 carre(vec3 p, vec3 centre, in float radius)
{
    return vec2(length(max(abs(p) - centre, 0.0)) - radius, 1.0);
}

float Segment(in vec3 p, in vec3 a, in vec3 b, in float radius, in float noise)
{
    float d;
    
    vec3 ab = (b - a);
    float d_ab = length(ab);
    
    vec3 ap = (p - a);
    vec3 bp = (p - b);
    
    vec3 u = (b - a) / length(b - a);
    float ah = dot((p - a), u);
    
    if (ah < 0.f)
        d = length2(ap);
    else if (ah > d_ab)
        d = length2(bp);
    else
        d = length2(ap) - pow(ah, 2.f);
        
    d = sqrt(d);
    
	return d - radius + noise;
}

// Implicit surface defining the terrain
// p : Point
float Implicit(in vec3 p)
{   
    // Terrain de base
    vec2 h = comp_height(p, Terrain( p.xy ));

    // Creusement pour le lac
    h = blend(h, comp_height(p, disque(p.xy, vec3(1.f, 1.f, -100.f), 800.f)));

    // Tunnel
    float noise_tunnel = 50.0 * Noise(p / 50.0) + 20.0 * Noise(p / 20.0);
    float segment = Segment(p, vec3(0.0, 300.0, 420.0), vec3(0.0, 1400.0, 420.0), 100.0, noise_tunnel);
    h.x = Diff(h.x, segment);

    // Avancement de la montagne
    float b, radius, noise, boule;

    radius = 150.0;
    noise = 50.* Noise(p / 50.) + 10. * Noise(p / 10.) + 20.0 * Noise(p / 20.0);

    segment = Segment(p, vec3(-25.0, -420.0, 580.0), vec3(125.0, -420.0, 580.0), radius, noise);
    boule = Sphere(p, vec3(60.0, -420, 710.0), 50.0, 10.0 * Noise(p / 10.0));

    b = BlendImplicite(boule, segment, 100.0);
    h.x = BlendImplicite(h.x, b, 500.0);

    // Zone supprimée de la montagne
    radius = 100.f;
    noise = 50.* Noise(p / 50.) + 10. * Noise(p / 10.);
    float d = Segment(p, vec3(125.f, -270.f, 455.f), vec3(-10.f, -250.f, 455.f), radius, noise);
    segment = Segment(p, vec3(200.f, -220.f, 455.f), vec3(-10.f, -200.f, 455.f), radius + 10.0, noise);
    // boule = Sphere(p, vec3(0.f, -200.f, 455.f), 20.0, 1.0);

    d = BlendImplicite(d, segment, 100.0);
    // d = BlendImplicite(d, boule, 100.0);
    
    h.x = Diff(h.x, d);

    // Correction de la suppression trop forte dans l'eau du lac
    // radius = 50.0;
    // b = Segment(p, vec3(200.f, -150.f, 315.f), vec3(-10.f, -170.f, 350.f), radius, 1.0);
    // h.x = BlendImplicite(h.x, b, 100.0);

    return h.x * h.y;
}

float EauImplicit(in vec3 p)
{
    vec2 w = comp_height(p, Eau(p.xy));
    return w.x * w.y;
}

// Sphere tracing
// ro, rd : Ray origin and direction
// t : Intersection depth
// i : Iteration count
bool Intersect(in vec3 ro, in vec3 rd, out float t,out int i)
{
    t = 0.0;
	for( i=0; i<Steps; i++ )
	{
        vec3 p = ro + t*rd;
		float h = Implicit(p);
        // 1 cm precision at 1 meter range, reduce precision as we move forward
		if ( abs(h)<(Epsilon*t)  ) return true;
        if ( t>view )  return false; 
		// Empirical Lipschitz constant with level of detail (the further, the larger steps)
        t += max(Epsilon,h*sqrt(1.0+8.0*t/view)/K);
	}

	return false;
}

// Sphere tracing
// ro, rd : Ray origin and direction
// t : Intersection depth
// i : Iteration count
bool EauIntersect(in vec3 ro, in vec3 rd, out float t,out int i)
{
    t = 0.0;
	for( i=0; i<Steps; i++ )
	{
        vec3 p = ro + t*rd;
		float h = EauImplicit(p);
        // 1 cm precision at 1 meter range, reduce precision as we move forward
		if ( abs(h)<(Epsilon*t)  ) return true;
        if ( t>view )  return false; 
		// Empirical Lipschitz constant with level of detail (the further, the larger steps)
        t += max(Epsilon,h*sqrt(1.0+8.0*t/view)/K);
	}

	return false;
}

// Analysis of the scalar field --------------------------------------------------------------------------

// Calculate object normal
// p : point
vec3 Normal(in vec3 p )
{
  float eps = 0.01;
  vec3 n;
  float v = Implicit(p);
  n.x = Implicit( vec3(p.x+eps, p.y, p.z) ) - v;
  n.y = Implicit( vec3(p.x, p.y+eps, p.z) ) - v;
  n.z = Implicit( vec3(p.x, p.y, p.z+eps) ) - v;
  return normalize(n);
}

vec3 EauNormal(in vec3 p )
{
  float eps = 0.01;
  vec3 n;
  float v = EauImplicit(p);
  n.x = Implicit( vec3(p.x+eps, p.y, p.z) ) - v;
  n.y = Implicit( vec3(p.x, p.y+eps, p.z) ) - v;
  n.z = Implicit( vec3(p.x, p.y, p.z+eps) ) - v;
  return normalize(n);
}

// Shading with number of steps
vec3 ShadeSteps(int n)
{
   float t=float(n)/(float(Steps-1));
   return 0.5+mix(vec3(0.05,0.05,0.5),vec3(0.65,0.39,0.65),t);
}

// Rendering 
// ro, rd : Ray origin and direction
// pip : Picture in picture boolean
vec4 Render( in vec3 ro, in vec3 rd, bool pip )
{
    // Light direction
    vec3 light1 = normalize( vec3(-0.8,-0.3,0.4) );
    
	vec3 col;
    float t;
    int it;
    bool b=Intersect( ro, rd, t , it);

    float tw;
    int itw;
    bool bw = EauIntersect( ro, rd, tw , itw);

    if( b==false && bw == false)
    {
        // sky		
        col = vec3(0.35,0.65,0.95) - rd.z*rd.z*2.5;
    }
	else
	{
        vec3 n;
        vec3 p = ro + t*rd;

        bool is_water = false;
        if (tw > t)
        {
            // mountains
            n = Normal( p );

            //vec3 ref = reflect( rd, n );
            //float fre = clamp( 1.0+dot(rd,n), 0.0, 1.0 );
            //vec3 hal = normalize(light1-rd);

            float ok_slope = 0.5;
            vec2 sud = vec2(1., 0.);
            float neige_orientation = dot(n.xy, sud) + clamp(-0.5, 0.5, Noise(p.xy));
            float ok_hauteur_snow = 625.0+(Noise(p.xy));
            float ok_hauteur_grass = 500.0;
            float ok_eau = 450.0;
            float slope = sqrt(n.x*n.x + n.y*n.y) / n.z ;

            col = vec3(0.2);

            if ((slope < ok_slope) && dot(n, vec3(0.0, 0.0, 1.0)) > 0.0)
                if (p.z >  ok_hauteur_snow && neige_orientation > 0.) // snow
                    col = vec3(1.0);
                else if (p.z < ok_hauteur_grass)
                    col = vec3(0., 0.5, 0.);

        }
        else
        {
            is_water = true;
            n = Normal(p); // On veut voir ce qu'il y a sous l'eau
            col = vec3(0.96, 0.85, 0.39);
            //col = mix(col,vec3(0.07, 0.32, 0.73),0.8 );
            //col = vec3(0.0, 0.0, 1.0);
        }


        float dif = 0.5*(1.0+dot( light1, n ));
        dif*=dif;
                
        col += dif*vec3(0.35);

        if (is_water)
            col = mix(col,vec3(0.07, 0.22, 0.83), 0.8);

		// fog
        // float fo = 1.0-exp(-pow(0.0005*t,1.5) );
        // vec3 fco = 0.65*vec3(0.4,0.65,1.0);
        // col = mix( col, fco, fo );

	}
    
    // Shading for iterations
    if (pip==true)
    {
    	return vec4(ShadeSteps(it),1.0);
    }
    else
    {
    // Gamma with square root
       return vec4( sqrt(col), t );
    }

}

mat3 moveCamera(float time, out vec3 ro)
{
	// Origin
    ro =  vec3(2000.0*cos(iMouse.x*0.01),2000.0*sin(iMouse.x*0.01),1500.0) ;
	
    // Target
    vec3 ta = vec3(0.0,0.0,500.0);
    
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(0.0, 0.0,1.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

vec2 RayDirection(in vec2 pixel, out bool pip)
{
    // Pixel coordinates
    vec2 p = (-iResolution.xy + 2.0*pixel)/iResolution.y;
   if (pip==true)
   {    
    const float fraction=1.0/4.0;
    // Picture in picture
    if ((pixel.x<iResolution.x*fraction) && (pixel.y<iResolution.y*fraction))
    {
        p=(-iResolution.xy*fraction + 2.0*pixel)/(iResolution.y*fraction);
        pip=true;
    }
       else
       {
           pip=false;
       }
   }
   return p;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
   // Time as function of the position of the mouse
    float time = iTime*0.25 + 4.0*iMouse.x/iResolution.x;

    // Camera transform
    vec3 ro; 
    mat3 cam = moveCamera( time, ro);   

    // Picture in picture
    bool pip=true;
    
    // Pixel coordinates
    vec2 p = RayDirection(fragCoord, pip);
   
    // Camera ray    
    vec3 rd = cam * normalize(vec3(p,3.0));
    
    // Render
    vec4 res = Render( ro, rd, pip );
     
    fragColor = vec4( res.xyz, 0.0 );
    
}
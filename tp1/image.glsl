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
float Terrain( in vec2 x )
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
	return -400.0+a0*a;
}

vec2 cercle(in vec2 x, in vec3 centre)
{   
    vec2 dir = x - centre.xy;
    float alpha = dir.x*dir.x + dir.y*dir.y;
    // g(x)
    alpha = (1 - (alpha / r*r));
    alpha = alpha * alpha * alpha;

    float height = centre.z;
    return vec2(height, alpha);
}

// Implicit surface defining the terrain
// p : Point
float Implicit(in vec3 p)
{
	float h = p.z - Terrain( p.xy );
    return h;
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
    if( b==false)
    {
        // sky		
        col = vec3(0.35,0.65,0.95) - rd.z*rd.z*2.5;
    }
	else
	{
        // mountains		
		vec3 p = ro + t*rd;
        vec3 n = Normal( p );

        vec3 ref = reflect( rd, n );
        float fre = clamp( 1.0+dot(rd,n), 0.0, 1.0 );
        vec3 hal = normalize(light1-rd);

        float ok_slope = 0.6;
        float ok_hauteur_snow = 600.0+(10.0*Noise(p.xy));
        float ok_hauteur_grass = 500.0;
        float slope = sqrt(n.x*n.x + n.y*n.y) / n.z ;

        if ((slope < ok_slope))
            if (p.z >  ok_hauteur_snow ) // snow
                col = vec3(1.0);
            else if (p.z < ok_hauteur_grass)
                col = vec3(0., 0.5, 0.);
        else
                col = vec3(0.2);
        
         // Pseudo diffuse lighting
		float dif = 0.5*(1.0+dot( light1, n ));
        dif*=dif;
        
        col += dif*vec3(0.35);

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
    ro =  vec3(2000.0*cos(iMouse.x*0.01),2000.0*sin(iMouse.x*0.01),1000.0) ;
	
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
// Texturing
// Eric Galin

// Texturing and noise ---------------------------------------------------------

// Hashing function
// Returns a random number in [-1,1]
// p : Vector in space
float Hash(in vec3 p)  
{
    p  = fract( p*0.3199+0.152 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

// Procedural value noise with cubic interpolation
// x : Point 
float Noise(in vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
  
    f = f*f*(3.0-2.0*f);
    // Could use quintic interpolation instead of cubic
    // f = f*f*f*(f*(f*6.0-15.0)+10.0);

    return mix(mix(mix( Hash(i+vec3(0,0,0)), 
                        Hash(i+vec3(1,0,0)),f.x),
                   mix( Hash(i+vec3(0,1,0)), 
                        Hash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( Hash(i+vec3(0,0,1)), 
                        Hash(i+vec3(1,0,1)),f.x),
                   mix( Hash(i+vec3(0,1,1)), 
                        Hash(i+vec3(1,1,1)),f.x),f.y),f.z);
}

// Hashing function
// Returns a random number in [-1,1]
// p : Vector in the plane
float Hash(in vec2 p)  
{
    p  = fract( p*0.3199+0.152 );
	p *= 17.0;
    return fract( p.x*p.y*(p.x+p.y) );
}


// Procedural value noise with cubic interpolation
// x : Point 
float Noise(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
  
    f = f*f*(3.0-2.0*f);
    // Could use quintic interpolation instead of cubic
    // f = f*f*f*(f*(f*6.0-15.0)+10.0);

    return mix(mix( Hash(i+vec2(0,0)), 
                        Hash(i+vec2(1,0)),f.x),
                   mix( Hash(i+vec2(0,1)), 
                        Hash(i+vec2(1,1)),f.x),f.y);
}

#include "kernel.h"

float hyperboloid_intersection(t_hyperboloid hyper, float3 ray_start, float3 ray_dir)
{
	float k1;
     float k2;
    float k3; 
	float a = hyper.a;
	float b = hyper.b;
	float c = hyper.c;
	float3 co = ray_start - hyper.center;
    k1 = ray_dir.x * ray_dir.x / (a * a) + ray_dir.y * ray_dir.y / (b * b) - ray_dir.z * ray_dir.z / (c * c);
    k2 = 2 * co.x * ray_dir.x / (a * a) + 2 * co.y * ray_dir.y / (b * b) - 2 * co.z * ray_dir.z / (c * c);
    k3 = co.x * co.x / (a * a) + co.y * co.y / (b * b) - co.z * co.z / (c * c) + 1.0f;
    float d = k2 * k2 - 4 * k1 * k3;
    if (d >= 0)
    {
        float t1 = (-k2 + sqrt(d)) / (2 * k1);
        float t2 = (-k2 - sqrt(d)) / (2 * k1);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
    } 
    return (0);
}

float2		swap(float2 ab)
{
	float tmp;

	tmp = ab.x;
	ab.x = ab.y;
	ab.y = tmp;
	return ab;
}

float _root3 (float x)
{
    float s = 1.0f;
    while ( x < 1.0f)
    {
        x *= 8.0f;
        s *= 0.5f;
    }
    while ( x > 8)
    {
        x *= 0.125f;
        s *= 2;
    }
    float r = 1.5f;
    r -= 1/3 * ( r - x / ( r * r ) );
    r -= 1/3 * ( r - x / ( r * r ) );
    r -= 1/3 * ( r - x / ( r * r ) );
    r -= 1/3 * ( r - x / ( r * r ) );
    r -= 1/3 * ( r - x / ( r * r ) );
    r -= 1/3 * ( r - x / ( r * r ) );
    return r * s;
}

float root3 ( float x )
{
    if ( x > 0 ) return _root3 ( x ); else
    if ( x < 0 ) return ((_root3(-x)) * (-1.0f)); else
    return 0;
}

float8 SolveP3(float8 x,float a,float b,float c)
{
	float a2 = a*a;
    float q  = (a2 - 3.0f*b)/9.0f; 
	float r  = (a*(2.0f*a2-9.0f*b) + 27.0f*c)/54.0f;
    float r2 = r*r;
	float q3 = q*q*q;
	float A,B;
	if (r2 <= (q3 + FLT_EPSILON)) {
		float t=r/sqrt(q3);
		if( t<-1.0f) t=-1.0f;
		if( t> 1.0f) t= 1.0f;
        t=acos(t);
        a/=3.0f; q=-2.0f*sqrt(q);
        x.s0=q*cos(t/3.0f)-a;
        x.s1=q*cos((t + M_PI * 2.0f)/3.0f)-a;
        x.s2=q*cos((t-  M_PI * 2.0f)/3.0f)-a;
		x.s4 = 3.0f;
        return(x);
    } else {
        A =-root3(fabs(r)+sqrt(r2-q3)); 
		if( r<0 ) A=-A;
		B = (A==0? 0 : q/A);

		a/=3;
		x.s0 =(A+B)-a;
        x.s1 = -0.5f * (A+B)-a;
        x.s2 = 0.5f * sqrt(3.0f) * (A-B);
		if (fabs(x.s2) < FLT_EPSILON )
		{
			x.s2 = x.s1;
			x.s4 = 2;
			return x;
		}
		x.s4 = 1;
        return (x);
    }
	return (x);
}

float2  CSqrt( float x, float y, float2 ab)
{
	float r  = sqrt(x*x+y*y);
	if( y==0 ) { 
		r = sqrt(r);
		if(x>=0) { ab.x=r; ab.y=0; } else { ab.x=0; ab.y=r; }
	} else {		// y != 0
		ab.x = sqrt(0.5f*(x+r));
		ab.y = 0.5f * y / ab.x;
	}
	return ab;
}

float8   SolveP4Bi(float8 x, float b, float d)
{
	float D = b*b-4.0f*d;
	if( D>=0 ) 
	{
		float sD = sqrt(D);
		float x1 = (-b+sD)/2.0f;
		float x2 = (-b-sD)/2.0f;
		if( x2>=0 )			
		{
			float sx1 = sqrt(x1);
			float sx2 = sqrt(x2);
			x.s0 = -sx1;
			x.s1 =  sx1;
			x.s2 = -sx2;
			x.s3 =  sx2;
			x.s4 = 4.0f;
			return x;
		}
		if( x1 < 0 )			
		{
			float sx1 = sqrt(-x1);
			float sx2 = sqrt(-x2);
			x.s0 =    0;
			x.s1 =  sx1;
			x.s2 =    0;
			x.s3 =  sx2;
			x.s4 = 0;
			return x;
		}
			float sx1 = sqrt( x1);
			float sx2 = sqrt(-x2);
			x.s0 = -sx1;
			x.s1 = sx1;
			x.s2 = 0;
			x.s3 = sx2;
			x.s4 = 2.0f;
			return x;
	} else {
		float sD2 = 0.5f*sqrt(-D);
		float2 ab;
		ab.x = x.s0;
		ab.y = x.s1;
		ab = CSqrt(-0.5f*b, sD2, ab);
		x.s0 = ab.x;
		x.s1 = ab.y;
		ab.x = x.s2;
		ab.y = x.s3;
		ab = CSqrt(-0.5f*b,-sD2, ab);
		x.s2 = ab.x;
		x.s3 = ab.y;
		x.s4 = 0;
		return x;
	}
}

float8  dblSort3(float8 abc) // make: a <= b <= c
{
	float2 k;
	if (abc.s0 > abc.s1 )
	{
		k.x = abc.s0;
		k.y = abc.s1;
		k = swap(k);	
		abc.s0 = k.x;
		abc.s1 = k.y;
	}
	if( abc.s2 < abc.s1 )
	{
		k.x = abc.s1;
		k.y = abc.s2;
		k = swap(k);		
		abc.s1 = k.x;
		abc.s2 = k.y;
		if (abc.s0 > abc.s1)
		{
			k.x = abc.s0;
			k.y = abc.s1;
			k = swap(k);	
			abc.s0 = k.x;
			abc.s1 = k.y;
		}
	}
	return abc;
}

float8   SolveP4De(float8 x, float b, float c, float d)	
{
	if( fabs(c)< FLT_EPSILON * (fabs(b)+fabs(d)) )
	{
		x = SolveP4Bi(x,b,d);
		return x;
	}
	x = SolveP3( x, 2.0f*b, b*b-4.0f*d, -c*c);
	int res3 = x.s4;
	if( res3>1.0f )
	{				
		x = dblSort3(x);
		if( x.s0 > 0)
		{
			float sz1 = sqrt(x.s0);
			float sz2 = sqrt(x.s1);
			float sz3 = sqrt(x.s2);
			if( c>0 )
			{
				x.s0 = (-sz1 -sz2 -sz3)/2.0f;
				x.s1 = (-sz1 +sz2 +sz3)/2.0f;
				x.s2 = (+sz1 -sz2 +sz3)/2.0f;
				x.s3 = (+sz1 +sz2 -sz3)/2.0f;
				x.s4 = 4.0f;
				return x;
			}
			x.s0 = (-sz1 -sz2 +sz3)/2.0f;
			x.s1 = (-sz1 +sz2 -sz3)/2.0f;
			x.s2 = (+sz1 -sz2 -sz3)/2.0f;
			x.s3 = (+sz1 +sz2 +sz3)/2.0f;
			x.s4 = 4.0f;
			return x;
		} 
		float sz1 = sqrt(-x.s0);
		float sz2 = sqrt(-x.s1);
		float sz3 = sqrt( x.s2);

		if( c>0 )
		{
			x.s0 = -sz3/2.0f;					
			x.s1 = ( sz1 -sz2)/2.0f;	
			x.s2 =  sz3/2.0f;
			x.s3 = (-sz1 -sz2)/2.0f;	
			x.s4 = 0;
			return x;
		}
		x.s0 =   sz3/2.0f;
		x.s1 = (-sz1 +sz2)/2.0f;
		x.s2 =  -sz3/2.0f;
		x.s3 = ( sz1 +sz2)/2.0f;
		x.s4 = 0;
		return x;
	} 
	if (x.s0 < 0) x.s0 = 0;
	float sz1 = sqrt(x.s0);
	float szr, szi;
	float2 ab;
	ab.x = szr;
	ab.y = szi;
	ab = CSqrt(x.s1, x.s2, ab); 
	szr = ab.x;
	szi = ab.y;
	if( c>0 )	
	{
		x.s0 = -sz1/2.0f-szr;			
		x.s1 = -sz1/2.0f+szr;			
		x.s2 = sz1/2.0f; 
		x.s3 = szi;
		x.s4 = 2.0f;
		return x;
	}
	x.s0 = sz1/2-szr;		
	x.s1 = sz1/2+szr;		
	x.s2 = -sz1/2;
	x.s3 = szi;
	x.s4 = 2;
	return x;
} 

float N4Step(float x, float a,float b,float c,float d)
{
	float fxs= ((4.0f*x+3.0f*a)*x+2.0f*b)*x+c;
	if (fxs == 0) return x;
	float fx = (((x+a)*x+b)*x+c)*x+d;
	return (x - fx / fxs);
} 

float8   SolveP4(float8 x,float a,float b,float c,float d) {
	float d1 = d + 0.25f * a * (0.25f * b * a - 3.0f/64.0f * a * a * a - c);
	float c1 = c + 0.5f * a *(0.25f * a * a - b);
	float b1 = b - 0.375f * a * a;


	x = SolveP4De(x, b1, c1, d1);
	int res = x.s4;
	if( res==4) { x.s0 -= a/4.0f; x.s1 -= a/4.0f; x.s2 -= a/4.0f; x.s3 -= a/4.0f; }
	else if (res==2) { x.s0-= a/4.0f; x.s1-= a/4.0f; x.s2-= a/4.0f; }
	else             { x.s0-= a/4.0f; x.s2-= a/4.0f; }
	if( res>0 )
	{
		x.s0 = N4Step(x.s0, a,b,c,d);
		x.s1 = N4Step(x.s1, a,b,c,d);
	}
	if( res>2.0f )
	{
		x.s2 = N4Step(x.s2, a,b,c,d);
		x.s3 = N4Step(x.s3, a,b,c,d);
	}
	return x;
}

float*	sort(private float *roots)
{
	for (int i = 3; i >= 0; i--)
	{	for (int j = 1; j <= i; j++)
			if (isgreater(roots[j - 1] ,roots[i]))
			{
				float tmp;
				tmp = roots[j - 1];
				roots[j - 1] = roots[j];
				roots[j] = tmp;
			}
	}
}

float torus_intersection(t_torus torus, float3 ray_start, float3 ray_dir)
{
    float c_4;
    float c_3;
    float c_2;
    float c_1;
    float c_0;

	float m = dot(ray_dir, ray_dir);
	float n = dot(ray_dir, (ray_start - torus.center));
	float o = dot((ray_start - torus.center), (ray_start - torus.center));
	float p = dot(ray_dir, torus.vec);
	float q = dot((ray_start - torus.center), torus.vec);
    c_4 = pow(m, 2);
    c_3 = 4.0f * m * n;
    c_2 = 4.0f * pow(m, 2) + 2.0f*m*o - 2.0f*(pow(torus.radius1, 2) + pow(torus.radius2, 2))*m + 4.0f*pow(torus.radius1, 2) *p * p;
    c_1 = 4.0f*n*o - 4.0f*(pow(torus.radius1, 2) + pow(torus.radius2, 2))*n + 8.0f*pow(torus.radius1, 2) *p*q;
    c_0 = o*o - 2.0f*(pow(torus.radius1, 2) + pow(torus.radius2, 2))*o + 4.0f*pow(torus.radius1, 2) *q * q + (pow(torus.radius1, 2) -pow(torus.radius2, 2)) * (pow(torus.radius1, 2) -pow(torus.radius2, 2));
	float8 roots;	
	float a =  c_3 / c_4;
	float b =  c_2 / c_4;
	float c =  c_1 / c_4;
	float d =  c_0 / c_4;

	roots = SolveP4(roots, a, b, c, d);
	int num_roots = roots.s4;
	if (num_roots == 0)
		return 0.0f;
	float root[4];
	root[0] = roots.s0;
	root[1] = roots.s1;
	root[2] = roots.s2;
	root[3] = roots.s3;
	for (int i = 0; i < 4; i++)
	{
		if (isless(root[i],0.0f))
		{
			root[i] = 1e5;
		}
	}
	sort(root);
	if (isless(root[0], 1e5))
	{
		return (root[0]);
	}
	return (0.0f);
}

float paraboloid_intersection(t_paraboloid parab, float3 ray_start, float3 ray_dir)
{
	float3 parab_dir;
    float a;
    float b;
    float c;
	float t1;
    float t2;

    parab_dir = ray_start - parab.center;
	//dir_norm = normalize(parab.center);
 	float3 dir_norm = parab.vec;
	//dir_norm = normalize(dir_norm);
    a = dot(ray_dir, ray_dir) - pow(dot(ray_dir, dir_norm), 2.0f);
    b = 2.0f * dot(ray_dir, parab_dir) - 2.0f * dot(ray_dir, dir_norm) * (dot(parab_dir, dir_norm) + 2.0f * parab.k);
    c = dot(parab_dir, parab_dir) - dot(parab_dir, dir_norm) * (dot(parab_dir, dir_norm) + 4.0f * parab.k);
    c = b * b - 4 * a * c;
	if (c >= 0)
	{
		c = sqrt(c);
		t1 = (-b + c) / (2.0f * a);
        t2 = (-b - c) / (2.0f * a);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
	}
    return (0);
}

float box_intersection(__global t_object_d *box, float3 ray_start, float3 ray_dir)
{
	float3 	t_min;
	float3 	t_max;
	float		a;
	float		b;
	float		c;
	float		t0;
	float		t1;
	int		face_in;
	int		face_out;
	float		tmin;

	a = 1.0f / ray_dir.x;
	if (a >= 0)
	{
		t_min.x = (box[0].primitive.box.a.x - ray_start.x) * a;
		t_max.x = (box[0].primitive.box.b.x - ray_start.x) * a;
	}
	else
	{
		t_min.x = (box[0].primitive.box.b.x - ray_start.x) * a;
		t_max.x = (box[0].primitive.box.a.x - ray_start.x) * a;
	}
	b = 1.0f / ray_dir.y;
	if (b >= 0)
	{
		t_min.y = (box[0].primitive.box.a.y - ray_start.y) * b;
		t_max.y = (box[0].primitive.box.b.y - ray_start.y) * b;
	}
	else
	{
		t_min.y = (box[0].primitive.box.b.y - ray_start.y) * b;
		t_max.y = (box[0].primitive.box.a.y - ray_start.y) * b;
	}
	c = 1.0f / ray_dir.z;
	if (c >= 0)
	{
		t_min.z = (box[0].primitive.box.a.z - ray_start.z) * c;
		t_max.z = (box[0].primitive.box.b.z - ray_start.z) * c;
	}
	else
	{
		t_min.z = (box[0].primitive.box.b.z - ray_start.z) * c;
		t_max.z = (box[0].primitive.box.a.z - ray_start.z) * c;
	}
	//find largest entering t value
	if (t_min.x > t_min.y)
	{
		t0 = t_min.x;
		face_in = (a >= 0.0f) ? 0 : 3;
	}
	else
	{
		t0 = t_min.y;
		face_in = (b >= 0.0f) ? 1 : 4;
	}
	if (t_min.z > t0)
	{
		t0 = t_min.z;
		face_in = (c >= 0.0f) ? 2 : 5;
	}
	//find smallest exiting t value
	if (t_max.x < t_max.y)
	{
		t1 = t_max.x;
		face_out = (a >= 0.0f) ? 3 : 0;
	}
	else
	{
		t1 = t_max.y;
		face_out = (b >= 0.0f) ? 4 : 1;
	}
	if (t_max.z < t1)
	{
		t1 = t_max.z;
		face_out = (c >= 0.0f) ? 5 : 2;
	}
	if (t0 < t1 && t1 > 0.1f)
	{
		if (t0 > 0.1f)
		{
			tmin = t0;
			box[0].primitive.box.face_hit = face_in;
		}
		else
		{
			tmin =  t1;
			box[0].primitive.box.face_hit = face_out;
		}
		return (tmin);
	}
	return (0);
}

float ellipsoid_intersection(t_ellipsoid el, float3 ray_start, float3 ray_dir)
{
   float k1;
    float k2;
    float k3;
	float a = el.a;
	float b = el.b;
	float c = el.c;
	float3 co = ray_start - el.center;
    k1 = ray_dir.x * ray_dir.x / (a * a) + ray_dir.y * ray_dir.y / (b * b) + ray_dir.z * ray_dir.z / (c * c);
    k2 = 2 * co.x * ray_dir.x / (a * a) + 2 * co.y * ray_dir.y / (b * b) + 2 * co.z * ray_dir.z / (c * c);
    k3 = co.x * co.x / (a * a) + co.y * co.y / (b * b) + co.z * co.z / (c * c) - 1;
    float d = k2 * k2 - 4 * k1 * k3;
    if (d >= 0)
    {
        float t1 = (-k2 + sqrt(d)) / (2 * k1);
        float t2 = (-k2 - sqrt(d)) / (2 * k1);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
    }
    return (0);
}

float cone_intersection(t_cone cone, float3 ray_start, float3 ray_dir)
{
	float t1;
    float t2;
    float b;
    float c;

    float3 dist = ray_start - cone.position;
	float a = dot(ray_dir, cone.vec);
	a = dot(ray_dir, ray_dir) - (1 + cone.angle * cone.angle) * a * a;
    b = 2 * (dot(ray_dir, dist) - (1 + cone.angle * cone.angle) * \
		dot(ray_dir, cone.vec) * dot(dist, cone.vec));
    c = dot(dist, cone.vec);
	c = dot(dist, dist) - (1 + cone.angle * cone.angle) * c * c;
	c = b * b - 4 * a * c;
	if (c >= 0)
	{
		c = sqrt(c);
		t1 = (-b + c) / (2 * a);
        t2 = (-b - c) / (2 * a);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
	}
	return (0);
}

float cylinder_intersection(t_cylinder cyl, float3 ray_start, float3 ray_dir)
{
	float t1;
    float t2;
    float b;
    float c;

    float3 dist = ray_start - cyl.position;
	float a = dot(ray_dir, cyl.vec);
	a = dot(ray_dir, ray_dir) - a * a;
    b = 2 * (dot(ray_dir, dist) - dot(ray_dir, cyl.vec) * \
		dot(dist, cyl.vec));
    c = dot(dist, cyl.vec);
	c = dot(dist, dist) - c * c - cyl.radius * cyl.radius;
    c = b * b - 4 * a * c;
	if (c >= 0)
	{
		c = sqrt(c);
		t1 = (-b + c) / (2 * a);
        t2 = (-b - c) / (2 * a);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
	}
	return (0);
}

float sphere_intersection(t_sphere sphere, float3 ray_start, float3 ray_dir)
{
    float a = dot(ray_dir, ray_dir);
    float b;
    float c;
    float t1;
    float t2;
    float3 dist = ray_start - sphere.center;
    b = 2 * dot(dist, ray_dir);
    c = dot(dist, dist) - (sphere.radius * sphere.radius);
    c = b * b - 4 * a * c;
    if (c >= 0)
    {
        c = sqrt(c);
        t1 = (-b + c) / (2 * a);
        t2 = (-b - c) / (2 * a);
        if ((t1 < t2 && t1 > 0) || (t2 < 0 && t1 >= 0))
            return (t1);
        if ((t2 < t1 && t2 > 0) || (t1 < 0 && t2 >= 0))
            return (t2);
        if (t2 == t1 && t2 >= 0)
            return (t2);
    }
    return (0);
}

float triangle_intersection(t_triangle triangle, float3 ray_start, float3 ray_dir)
{
	float3 edge1;
	float3 edge2;
    float3 vec1;
	float3 vec2;
	float3 vec3;
    float det;
    float uv1;
	float uv2;

    edge1 = triangle.vertex[1] - triangle.vertex[0];
	edge2 = triangle.vertex[2] - triangle.vertex[0];
	vec1 = cross(ray_dir, edge2);
	det = dot(edge1, vec1);
	if (det < 1e-8 && det > -1e-8)
		return (0);
	det = 1 / det;
	vec2 = ray_start - triangle.vertex[0];
	uv1 = dot(vec2, vec1) * det;
	if (uv1 < 0 || uv1 > 1)
		return (0);
	vec3 = cross(vec2, edge1);
	uv2 = dot(ray_dir, vec3) * det;
	if (uv2 < 0 || uv1 + uv2 > 1)
		return (0);
	float res;
	res = dot(edge2, vec3) * det;
	return (res);
}

float plane_intersection(t_plane plane, float3 ray_start, float3 ray_dir)
{
	float k1;
	float k2;

    if ((dot(ray_dir, plane.normal)) == 0)
		return (0);
	k1 = dot(ray_start, plane.normal) + plane.d;
	k2 = dot(ray_dir, plane.normal);
	if (k1 == 0 || (k1 < 0 && k2 < 0) || (k1 > 0 && k2 > 0))
		return (0);
	k1 = -k1 / k2;
	return (k1);
}

int			in_shadow(int index, float3 l, __global float3 *intersection_buf, \
						int obj_nmb, __global t_object_d *obj)
{ 
	float3	ray_start;
	int		i;
	float	t;

	i = 0;
	ray_start = l * 0.001f;
	ray_start += intersection_buf[index];
	while (i < obj_nmb)
	{
		if (obj[i].type == SPHERE)
			t = sphere_intersection(obj[i].primitive.sphere, ray_start, l);
		if (obj[i].type == TRIANGLE)
			t = triangle_intersection(obj[i].primitive.triangle, ray_start, l);
		if (obj[i].type == PLANE)
			t = plane_intersection(obj[i].primitive.plane, ray_start, l);
		if (obj[i].type == CONE)
			t = cone_intersection(obj[i].primitive.cone, ray_start, l);
		if (obj[i].type == CYLINDER)
			t = cylinder_intersection(obj[i].primitive.cylinder, ray_start, l);
		if (obj[i].type == ELLIPSOID)
			t = ellipsoid_intersection(obj[i].primitive.ellipsoid, ray_start, l);
		if (obj[i].type == BOX)
			t = box_intersection(&obj[i].primitive.box, ray_start, l);
		if (obj[i].type == PARABOLOID)
			t = paraboloid_intersection(obj[i].primitive.paraboloid, ray_start, l);
		 if (obj[i].type == TORUS)
			t = torus_intersection(obj[i].primitive.torus, ray_start, l);
		if (obj[i].type == HYPERBOLOID)
			t = hyperboloid_intersection(obj[i].primitive.hyperboloid, ray_start, l);
		if (t < 1.0f && t > 0.00001f)
			break ;
		i++;
	}
	if (t < 1.0f && t > 0.0001f)
		return (1); 
	return (0);
}

float		get_specular(int index, int j, float3 l, \
						__global float3 *normal_buf, \
						__global t_light *light, \
 						__global t_material *material_buf, \
						__global float3 *ray_buf)
{
	float		n;
	float		r;
	float		i;
 	float3		rad;
	float3		d;
	float3		lb;

	lb = l / length(l);
	i = 0;
	n = dot(normal_buf[index], lb);
	rad = normal_buf[index] * 2.0f;
	rad = rad * n;
	rad = rad - lb;
	d.x = -ray_buf[index].x;
	d.y = -ray_buf[index].y;
	d.z = -ray_buf[index].z;
	r = dot(rad, d);
	if (r > 0)
		i += light[j].intensity * pow((float)r / \
		(length(rad) * length(d)), \
		material_buf[index].specular);
	return (i);
}

float3		get_light_vec(int index, int j, __global float3 *intersection_buf, __global t_light *light)
{
	float3 light_vec;

	light_vec = 0;
	if (light[j].type == POINT)
		light_vec = light[j].position - intersection_buf[index];
	if (light[j].type == DIRECTIONAL)
		light_vec = light[j].direction;
	return (light_vec);
}

t_color		reflection_color(__global t_color *frame_buf, \
							__global float3 *ray_buf, \
                            __global float3 *intersection_buf, \
                            __global float3 *normal_buf, \
                            __global int *index_buf, \
                            __global t_material *material_buf, \
                            __global t_object_d *obj, \
                            __global t_light *light, \
							int light_nmb, \
							int index, int obj_nmb, int bounce_cnt, \
							__global t_material *prev_material_buf)
{
	float		i;
	float3		l;
	float		n_dot_l;
	int			j;

	j = -1;
	i = 0;
	while (++j < light_nmb)
	{
		if (light[j].type == AMBIENT)
			i += light[j].intensity;
		else
		{
			l = get_light_vec(index, j, intersection_buf, light);
			n_dot_l = dot(normal_buf[index], l);
			if (!(in_shadow(index, l, intersection_buf, obj_nmb, obj)) && n_dot_l > 0)
			{
				if (material_buf[index].specular != -1)
					i += get_specular(index, j, l, normal_buf, light, material_buf, ray_buf);
				i += (light[j].intensity * n_dot_l) / length(l);
			}
		}
	}
	i = i > 1 ? 1 : i;
	t_color result;
	result.red = material_buf[index].color.red * i;
	result.green = material_buf[index].color.green * i;
	result.blue = material_buf[index].color.blue * i;
/* 	if (bounce_cnt > 0)
	{
		result.red = (1 - prev_material_buf[index].reflection) * result.red + prev_material_buf[index].reflection * frame_buf[index].red;
		result.green = (1 - prev_material_buf[index].reflection) * result.green + prev_material_buf[index].reflection * frame_buf[index].green;
		result.blue = (1 - prev_material_buf[index].reflection) * result.blue + prev_material_buf[index].reflection * frame_buf[index].blue;
	} */
	return (result);
}

__kernel void get_frame_buf_cl(__global t_color *frame_buf, \
                            __global float3 *ray_buf, \
                            __global float3 *intersection_buf, \
                            __global float3 *normal_buf, \
                            __global int *index_buf, \
                            __global t_material *material_buf, \
                            __global t_object_d *obj, \
                            __global t_light *light, \
                            int light_nmb,\
							int obj_nmb, int bounce_cnt, \
							__global t_material *prev_material_buf, \
							int is_refractive, __global t_color *refl_buf, \
							__global t_color *refr_buf, __global int *orig_index_buf, \
							int has_refraction)
{
    int i = get_global_id(0);
	int j = index_buf[i];
	t_color buf;
	float check;
	if (j != -1 && bounce_cnt == 0 && !is_refractive)
	{
		frame_buf[i] = reflection_color(frame_buf, ray_buf, intersection_buf, \
										normal_buf, index_buf, material_buf, \
										obj, light, light_nmb, i, obj_nmb, bounce_cnt, prev_material_buf);
	}
	else if (j != -1 && bounce_cnt == 0 && is_refractive && obj[orig_index_buf[i]].refraction > 0.0f)
	{
		refr_buf[i] = reflection_color(frame_buf, ray_buf, intersection_buf, \
										normal_buf, index_buf, material_buf, \
										obj, light, light_nmb, i, obj_nmb, bounce_cnt, prev_material_buf);
		//frame_buf[i] = refr_buf[i];
	}
	else if (j != -1 && bounce_cnt > 0 && !is_refractive && obj[orig_index_buf[i]].reflection > 0.0f)
	{
		refl_buf[i] = reflection_color(frame_buf, ray_buf, intersection_buf, \
										normal_buf, index_buf, material_buf, \
										obj, light, light_nmb, i, obj_nmb, bounce_cnt, prev_material_buf);
		if (has_refraction == 0)
		{
			check = (1.0f - obj[orig_index_buf[i]].reflection) * refl_buf[i].red + obj[orig_index_buf[i]].reflection * frame_buf[i].red;
 			frame_buf[i].red = check > 255 ? 255 : check;
			check = (1.0f - obj[orig_index_buf[i]].reflection) * refl_buf[i].green + obj[orig_index_buf[i]].reflection * frame_buf[i].green;
			frame_buf[i].green = check > 255 ? 255 : check;
			check  = (1.0f - obj[orig_index_buf[i]].reflection) * refl_buf[i].blue + obj[orig_index_buf[i]].reflection * frame_buf[i].blue;
			frame_buf[i].blue = check > 255 ? 255 : check;
		}
	} 
/* 	else if (j != -1 && bounce_cnt > 0 && is_refractive)
	{
		// второй случай
		buf = reflection_color(frame_buf, ray_buf, intersection_buf, \
										normal_buf, index_buf, material_buf, \
										obj, light, light_nmb, i, obj_nmb, bounce_cnt, prev_material_buf);
		frame_buf[i].red = frame_buf[i].red / 2 + buf.red / 2;
		frame_buf[i].green = frame_buf[i].green / 2 + buf.green / 2;
		frame_buf[i].blue = frame_buf[i].blue / 2 + buf.blue / 2;
	} */
	else if (j == -1 && bounce_cnt == 0 && !is_refractive)
	{
		frame_buf[i].red = 0;
		frame_buf[i].green = 0;
		frame_buf[i].blue = 0;
		frame_buf[i].alpha = 255;
	}
	if (bounce_cnt > 0 && obj[orig_index_buf[i]].refraction > 0.0f)
	{
		//printf("kr = %f\n", material_buf[i].kr);
		float check = refl_buf[i].red * material_buf[i].kr + refr_buf[i].red * (1.0 - material_buf[i].kr) * obj[orig_index_buf[i]].transparency;
		buf.red = check > 255 ? 255 : check;
		check = refl_buf[i].green  * material_buf[i].kr + refr_buf[i].green * (1.0 - material_buf[i].kr) * obj[orig_index_buf[i]].transparency;
		buf.green = check > 255 ? 255 : check;
		check = refl_buf[i].blue * material_buf[i].kr + refr_buf[i].blue * (1.0 - material_buf[i].kr) * obj[orig_index_buf[i]].transparency;
		buf.blue = check > 255 ? 255 : check;
		
		//check = (buf.red ) * 0;
		check = buf.red + frame_buf[i].red * material_buf[i].kr * (1.0f - obj[orig_index_buf[i]].transparency);
		frame_buf[i].red = check > 255 ? 255 : check;
		//check =	(buf.green) * 0;
		check = buf.green + frame_buf[i].green * material_buf[i].kr * (1.0f - obj[orig_index_buf[i]].transparency);
		frame_buf[i].green = check > 255 ? 255 : check;
		//check =	(buf.blue)* 0;
		check = buf.blue + frame_buf[i].blue * material_buf[i].kr * (1.0f - obj[orig_index_buf[i]].transparency);
		frame_buf[i].blue = check > 255 ? 255 : check;
		
	}
}

/* frame_buf[i].red = frame_buf[i].red;
		frame_buf[i].blue = frame_buf[i].blue;
		frame_buf[i].green = frame_buf[i].green; */
		//float check = refl_buf[i].red * material_buf[i].kr + refr_buf[i].red * (1.0 - material_buf[i].kr);
/* 		frame_buf[i].red = check > 255 ? 255 : check;
		check = refl_buf[i].green * material_buf[i].kr + refr_buf[i].green * (1.0 - material_buf[i].kr);
		frame_buf[i].green = check > 255 ? 255 : check;
		check = refl_buf[i].blue * material_buf[i].kr + refr_buf[i].blue * (1.0 - material_buf[i].kr);
		frame_buf[i].blue = check > 255 ? 255 : check; */
		/* if (obj[orig_index_buf[i]].refraction > 0.0f)
		{
			printf("asldk");
			/* frame_buf[i].red = refl_buf[i].red;
			frame_buf[i].blue = refl_buf[i].blue;
			frame_buf[i].green = refl_buf[i].green;
		} */
		/* check = color.red * 0.2 * frame_buf[i].red;
		frame_buf[i].red = check > 255 ? 255 : check;
		check =	color.green * 0.2 * frame_buf[i].green;
		frame_buf[i].green = check > 255 ? 255 : check;
		check =	color.blue * 0.2 * frame_buf[i].blue;
		frame_buf[i].blue = check > 255 ? 255 : check; */
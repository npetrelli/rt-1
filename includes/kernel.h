#ifndef KERNEL_H
# define KERNEL_H

enum light_type{
	POINT,
	AMBIENT,
	DIRECTIONAL
};

typedef struct          s_basis
{
    float3       u;
    float3       v;
    float3       w;
}                      	t_basis;

typedef	struct		s_light
{
	float3			position;
	float3			direction;
	float			intensity;
	int				type;
}					t_light;

typedef struct 		s_color
{
	uchar			red;
	uchar			green;
	uchar			blue;
	uchar			alpha;
}					t_color;

typedef	struct		s_material
{
	t_color			color;
	float			specular;
}					t_material;

typedef struct  s_sphere
{
    float3      center;
    float       radius;
}               t_sphere;

typedef struct 	s_plane
{
    float3		normal;
	float3		point;
	float		d;
}				t_plane;

typedef struct 		s_cylinder
{
    float3		position;
	float3		vec;
	float		radius;
}					t_cylinder;

typedef struct 		s_cone
{
    float3		    position;
	float3		    vec;
	float			angle;
}					t_cone;

typedef struct 		s_triangle
{
    float3		vertex[3];
	float3		normal;
}					t_triangle;

typedef	struct 		s_ellipsoid
{
	float3 			abc;
	float3			center;
}					t_ellipsoid;

typedef	struct 		s_box
{
	float3 			a;
	float3			b;
}					t_box;

typedef struct		s_paraboloid
{
	float			k;
	float3			center;
}					t_paraboloid;

typedef	union			primitive
{
	t_cylinder			cylinder;
	t_cone				cone;
	t_sphere			sphere;
	t_plane				plane;
	t_triangle			triangle;
	t_ellipsoid			ellipsoid;
	t_paraboloid		paraboloid;
	t_box				box;
}						t_primitive;

typedef	struct		s_cutting_surface
{
    int 			type;
	t_sphere		sphere;
	t_plane			plane;
	t_triangle		triangle;
	t_cone			cone;
	t_cylinder		cylinder;
}					t_cutting_surface;

enum object_type {
	SPHERE,
	CONE,
	TRIANGLE,
	CYLINDER,
	PLANE,
	ELLIPSOID,
	HYPERBOLOID,
	PARABOLOID,
	BOX
};

typedef struct 			s_object3d_d
{
	t_primitive			primitive;
	t_basis				basis;
	float3				rotation;
	t_color				color;
	float				specular;
	float				roughness;
	float				refraction;
	float				reflection;
	int					color_disrupt;
	int 				type;
	int					texture_id;
	int 				texture_size;
	int					texture_width;
	int					texture_height;
	int					l_size;
}						t_object_d;

float cone_intersection(t_cone , float3 , float3);
float cylinder_intersection(t_cylinder , float3 , float3 );
float plane_intersection(t_plane , float3 , float3 );
float sphere_intersection(t_sphere , float3 , float3 );
float triangle_intersection(t_triangle , float3 , float3 );

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/07 15:39:58 by hunnamab          #+#    #+#             */
/*   Updated: 2020/11/10 13:36:22 by pmetron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H
# include <stdint.h>

typedef	struct			s_scene t_scene;

enum object_type {
	SPHERE,
	CONE,
	TRIANGLE,
	CYLINDER,
	PLANE,
	ELLIPSOID,
	HYPERBOLOID,
	PARABOLOID
};

typedef struct 			s_scene_device
{
	cl_mem				ray_buf;
	cl_mem				intersection_buf;
	cl_mem				viewport;
	cl_mem				index_buf;
	cl_mem				depth_buf;
	cl_mem				normal_buf;
	cl_mem				material_buf;
	cl_mem				camera;
}						t_scene_device;

typedef	struct 			s_cl_data
{
	cl_device_id		device_id; // compute device id
	cl_context			context; // compute context
	cl_command_queue	commands; // compute command queue
	cl_program			*programs;
	cl_kernel 			*kernels; // compute kernel
	t_scene_device		scene;
}						t_cl_data;

typedef struct		s_sdl
{
	SDL_Window		*win;
	SDL_Renderer	*renderer;
	SDL_Event		event;
}					t_sdl;

typedef struct		s_ray
{
	cl_float3		start;
	cl_float3		dir;
}					t_ray;

typedef struct __attribute__ ((aligned (256))) _s_color
{
	unsigned char			red;
	unsigned char			green;
	unsigned char			blue;
	unsigned char			alpha;
}					t_color;

typedef	struct		s_light
{
	float			intensity;
	cl_float3		position;
	cl_float3		direction;
	const char		*type;
}					t_light;

typedef struct __attribute__ ((aligned (256))) _s_sphere
{
	float			radius;
	cl_float3		center;
}					t_sphere;

typedef struct __attribute__ ((aligned (256))) _s_plane
{
	float			d;
	cl_float3		normal;
	cl_float3		point;
}					t_plane;

typedef struct __attribute__ ((aligned (256))) _s_cylinder
{
	float			radius;
	cl_float3		position;
	cl_float3		vec;
}					t_cylinder;

typedef struct __attribute__ ((aligned (256))) _s_cone
{
	float			angle;
	cl_float3		position;
	cl_float3		vec;
}					t_cone;

typedef struct __attribute__ ((aligned (256))) _s_triangle
{
	cl_float3		normal;
	cl_float3		*vertex;
}					t_triangle;

typedef struct		s_camera
{
	cl_float3		position;
	float			rotation[3];
}					t_camera;

typedef	struct		s_material
{
	t_color			color;
	float			specular;
}					t_material;

typedef	struct		s_object3d
{
	void			*data;
	const char		*tag;
	float			rotation[3];
	t_color			color;
	float			specular;
	int 			type;
	void			(*get_normal)(struct s_scene *, int, int);
	void			(*intersect)(t_scene *, int);
	void			(*clear_obj)(struct s_object3d *);
}					t_object;

typedef struct __attribute__ ((aligned (256))) _s_object3d_d
{
	int 			type;
	float			specular;
	t_sphere		sphere;
	t_plane			plane;
	t_triangle		triangle;
	t_cone			cone;
	t_cylinder		cylinder;
	t_color			color;
}					t_object_d;

struct		s_scene
{
	t_cl_data		cl_data;
	t_object		**objs;	
	int				obj_nmb;
	cl_float3		*normal_buf;
	t_material		*material_buf;
	cl_float3		*intersection_buf;
	cl_float3		*ray_buf;
	t_light			**light;
	int				light_nmb;
	t_camera		camera;
	cl_float3		*viewport;
	int				*index_buf;
	float			*depth_buf;
	int				mode; // 0 - default, 1 - normal, 2 - depth, 3 - flat_light
	void			(*init[4])(struct s_scene *);
	void			(*draw[4])(t_sdl *, struct s_scene *);
};

#endif

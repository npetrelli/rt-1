/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/07 14:21:11 by pmetron           #+#    #+#             */
/*   Updated: 2021/01/26 19:55:05 by pmetron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	init_deepth(t_scene *scene)
{
	float	**matrix;
	int		x;

	x = -1;
	scene->viewport = protected_malloc(sizeof(cl_float3), (WID * HEI));
	get_viewport(scene);
	scene->ray_buf = protected_malloc(sizeof(cl_float3), (WID * HEI));
	get_rays_arr(scene);
	scene->depth_buf = protected_malloc(sizeof(float), WID * HEI);
	scene->index_buf = protected_malloc(sizeof(int), WID * HEI);
	scene->frame_buf = protected_malloc(sizeof(t_color), (WID * HEI));
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_TRUE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
}

void	init_default(t_scene *scene)
{
	float	**matrix;
	int		x;

	x = -1;
	scene->viewport = protected_malloc(sizeof(cl_float3), (WID * HEI));
	scene->ray_buf = protected_malloc(sizeof(cl_float3), (WID * HEI));
	scene->normal_buf = protected_malloc(sizeof(cl_float3), WID * HEI);
	scene->material_buf = protected_malloc(sizeof(t_material), WID * HEI);
	scene->intersection_buf = protected_malloc(sizeof(cl_float3), WID * HEI);
	scene->index_buf = protected_malloc(sizeof(int), WID * HEI);
	scene->depth_buf = protected_malloc(sizeof(float), WID * HEI);	
	scene->frame_buf = protected_malloc(sizeof(t_color), (WID * HEI));
}

void	init_raycast(t_scene *scene)
{
	float	**matrix;
	int		x;

	x = -1;
	scene->viewport = protected_malloc(sizeof(cl_float3), (WID * HEI));
	get_viewport(scene);
	scene->ray_buf = protected_malloc(sizeof(cl_float3), (WID * HEI));
	get_rays_arr(scene);
	scene->depth_buf = protected_malloc(sizeof(float), WID * HEI);
	scene->material_buf = protected_malloc(sizeof(t_material), WID * HEI);
	scene->index_buf = protected_malloc(sizeof(int), WID * HEI);
	scene->intersection_buf = protected_malloc(sizeof(cl_float3), WID * HEI);
	scene->frame_buf = protected_malloc(sizeof(t_color), (WID * HEI));
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_TRUE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
	
}

void	refresh_scene(t_scene *scene)
{
	//scene->draw[scene->mode](sdl, scene);
}

void	init_scene(t_scene *scene)
{
	scene->init[0] = &init_default;
	scene->init[1] = &init_default;
	scene->init[2] = &init_deepth;
	scene->init[3] = &init_raycast;
	scene->draw[0] = &draw_scene;
	scene->draw[1] = &draw_normal_buf;
	scene->draw[2] = &draw_deepth_buf;
	scene->draw[3] = &draw_raycast;
	scene->filter[0] = &gauss_filter;
	scene->filter[1] = &sepia_filter;
	scene->filter[2] = &gray_scale;
	scene->normal_buf = NULL;
	scene->material_buf = NULL;
	scene->intersection_buf = NULL;
	scene->ray_buf = NULL;
	scene->viewport = NULL;
	scene->index_buf = NULL;
	scene->depth_buf = NULL;
	scene->init[scene->mode](scene);
}
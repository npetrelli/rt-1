/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/07 14:34:50 by pmetron           #+#    #+#             */
/*   Updated: 2021/01/27 20:46:00 by pmetron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	draw_scene(t_sdl *sdl, t_scene *scene)
{
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;
	float				**matrix;

	x = -1;
	y = -1;
	i = 0;
	scene->bounce_cnt = 0;
	scene->max_bounces = 2;

	size_t global = WID * HEI;
	size_t local;
	
	get_viewport(scene);
	get_rays_arr(scene);
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
	while (j < WID * HEI)
	{
		scene->intersection_buf[j] = scene->camera.position;
		j++;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.intersection_buf, CL_FALSE, 0, sizeof(cl_float3) * global, scene->intersection_buf, 0, NULL, NULL);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, scene->cl_data.scene.normal_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	x = -1;
	while (++x < WID * HEI)
	{
		scene->index_buf[x] = -1;
		scene->depth_buf[x] = 100000000;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * global, scene->index_buf, 0, NULL, NULL);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * global, scene->depth_buf, 0, NULL, NULL);
	while (scene->bounce_cnt < scene->max_bounces)
	{
		get_closest_points(scene, 0);
		get_intersection_buf(scene);
		clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.normal_buf, scene->cl_data.scene.ray_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
		get_normal_buf(scene);
		get_material_buf(scene);
		get_frame_buf(scene);
		scene->bounce_cnt++;
	}
	if (scene->filter_type != DEFAULT)
		scene->filter[scene->filter_type](&scene->filter_data);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.frame_buf, 0, 0, sizeof(t_color) * global, scene->frame_buf, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
	while (++y < HEI)
	{
		while (++x < WID)
		{
			i = y * WID + x;
			SDL_SetRenderDrawColor(sdl->renderer, \
			scene->frame_buf[i].red, scene->frame_buf[i].green, scene->frame_buf[i].blue, 255);
			SDL_RenderDrawPoint(sdl->renderer, x, y);
		}
		x = -1;
	}
	draw_ui(sdl->renderer, scene->rt_ui);
	SDL_RenderPresent(sdl->renderer);
}

void	draw_normal_buf(t_sdl *sdl, t_scene *scene)
{
	int		xyi[3];
	t_color	color;
	float				**matrix;
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;

	x = -1;
	y = -1;
	i = 0;
	get_viewport(scene);
	get_rays_arr(scene);
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
	while (j < WID * HEI)
	{
		scene->intersection_buf[j] = scene->camera.position;
		j++;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.intersection_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->intersection_buf, 0, NULL, NULL);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, scene->cl_data.scene.normal_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	x = -1;
	while (++x < WID * HEI)
	{
		scene->index_buf[x] = -1;
		scene->depth_buf[x] = 100000000;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * WID * HEI, scene->index_buf, 0, NULL, NULL);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * WID * HEI, scene->depth_buf, 0, NULL, NULL);
	get_closest_points(scene, 0);
	get_intersection_buf(scene);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.normal_buf, scene->cl_data.scene.ray_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	get_normal_buf(scene);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * (WID * HEI), scene->index_buf, 0, NULL, NULL);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.normal_buf, CL_FALSE, 0, sizeof(cl_float3) * (WID * HEI), scene->normal_buf, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
	xyi[1] = -1;
	while (++xyi[1] < HEI)
	{
		xyi[0] = -1;
		while (++xyi[0] < WID)
		{
			xyi[2] = xyi[1] * WID + xyi[0];
			if (scene->index_buf[xyi[2]] != -1)
			{
				color.red = (scene->normal_buf[xyi[2]].x * 0.5 + 0.5) * 100;
				color.green = (scene->normal_buf[xyi[2]].y * 0.5 + 0.5) * 100;
				color.blue = (scene->normal_buf[xyi[2]].z * 0.5 + 0.5) * 100;
			}
			else
				set_color_zero(&color);
			SDL_SetRenderDrawColor(sdl->renderer, \
			color.red, color.green, color.blue, 255);
			SDL_RenderDrawPoint(sdl->renderer, xyi[0], xyi[1]);
		}
	}
	draw_ui(sdl->renderer, scene->rt_ui);
	SDL_RenderPresent(sdl->renderer);
}

void	draw_deepth_buf(t_sdl *sdl, t_scene *scene)
{
	int		xyi[3];
	t_color	color;
	float				**matrix;
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;
	cl_int err;
	x = -1;
	y = -1;
	i = 0;
	
	get_viewport(scene);
	get_rays_arr(scene);
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	err = clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
	while (j < WID * HEI)
	{
		scene->intersection_buf[j] = scene->camera.position;
		j++;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.intersection_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->intersection_buf, 0, NULL, NULL);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, scene->cl_data.scene.normal_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	x = -1;
	while (++x < WID * HEI)
	{
		scene->index_buf[x] = -1;
		scene->depth_buf[x] = 100000000;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * WID * HEI, scene->index_buf, 0, NULL, NULL);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * WID * HEI, scene->depth_buf, 0, NULL, NULL);
	get_closest_points(scene, 0);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * (WID * HEI), scene->index_buf, 0, NULL, NULL);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * WID * HEI, scene->depth_buf, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
	xyi[1] = -1;
	while (++xyi[1] < HEI)
	{
		xyi[0] = -1;
		while (++xyi[0] < WID)
		{
			xyi[2] = xyi[1] * WID + xyi[0];
			if (scene->index_buf[xyi[2]] != -1)
			{
				color.red = scene->depth_buf[xyi[2]] > 255 ? \
				255 : scene->depth_buf[xyi[2]];
				color.green = color.red;
				color.blue = color.red;
			}
			else
				set_color_zero(&color);
			SDL_SetRenderDrawColor(sdl->renderer, \
			color.red, color.green, color.blue, 255);
			SDL_RenderDrawPoint(sdl->renderer, xyi[0], xyi[1]);
		}
	}
	draw_ui(sdl->renderer, scene->rt_ui);
	SDL_RenderPresent(sdl->renderer);
}

void	draw_raycast(t_sdl *sdl, t_scene *scene)
{
	int		x = -1;
	int		y;
	t_color	color;
	int		i;
	int		j = 0;
	float	**matrix;

	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * (WID * HEI), scene->index_buf, 0, NULL, NULL);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, CL_FALSE, 0, sizeof(t_material) * WID * HEI, scene->material_buf, 0, NULL, NULL);
	get_viewport(scene);
	get_rays_arr(scene);
	matrix = get_rotation_matrix(scene->camera.rotation);
	while (++x < WID * HEI)
		transform(&scene->ray_buf[x], matrix, 1);
	matr_free(matrix, 4);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->ray_buf, 0, NULL, NULL);
	while (j < WID * HEI)
	{
		scene->intersection_buf[j] = scene->camera.position;
		j++;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.intersection_buf, CL_FALSE, 0, sizeof(cl_float3) * WID * HEI, scene->intersection_buf, 0, NULL, NULL);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, scene->cl_data.scene.normal_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	x = -1;
	while (++x < WID * HEI)
	{
		scene->index_buf[x] = -1;
		scene->depth_buf[x] = 100000000;
	}
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * WID * HEI, scene->index_buf, 0, NULL, NULL);
	clEnqueueWriteBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * WID * HEI, scene->depth_buf, 0, NULL, NULL);
	get_closest_points(scene, 0);
	get_intersection_buf(scene);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.normal_buf, scene->cl_data.scene.ray_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	get_normal_buf(scene);
	get_material_buf(scene);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, CL_FALSE, 0, sizeof(t_material) * WID * HEI, scene->material_buf, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
	x = -1;
	y = -1;
	i = 0;
	while (++y < HEI)
	{
		while (++x < WID)
		{
			i = y * WID + x;
			if (scene->index_buf[i] != -1)
				color = scene->material_buf[i].color;
			else
				set_color_zero(&color);
			SDL_SetRenderDrawColor(sdl->renderer, \
			color.red, color.green, color.blue, 255);
			SDL_RenderDrawPoint(sdl->renderer, x, y);
		}
		x = -1;
	}
	draw_ui(sdl->renderer, scene->rt_ui);
	SDL_RenderPresent(sdl->renderer);
}

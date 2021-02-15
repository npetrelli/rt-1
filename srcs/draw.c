/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hunnamab <hunnamab@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 00:18:09 by npetrell          #+#    #+#             */
/*   Updated: 2021/02/15 18:17:16 by hunnamab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	get_refraction_ray(t_scene *scene)
{
	size_t global = WID * HEI;
	size_t local;
	cl_mem swap_pointer;
	clSetKernelArg(scene->cl_data.kernels[15], 0, sizeof(cl_mem), &scene->cl_data.scene.ray_buf);
	clSetKernelArg(scene->cl_data.kernels[15], 1, sizeof(cl_mem), &scene->cl_data.scene.index_buf);
	clSetKernelArg(scene->cl_data.kernels[15], 2, sizeof(cl_mem), &scene->cl_data.scene.normal_buf);
	clSetKernelArg(scene->cl_data.kernels[15], 3, sizeof(cl_mem), &scene->cl_data.scene.exception_buf);
	clSetKernelArg(scene->cl_data.kernels[15], 4, sizeof(cl_mem), &scene->cl_data.scene.obj);
	clSetKernelArg(scene->cl_data.kernels[15], 5, sizeof(cl_mem), &scene->cl_data.scene.material_buf);
	clGetKernelWorkGroupInfo(scene->cl_data.kernels[15], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[15], 1, NULL, &global, &local, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
	swap_pointer = scene->cl_data.scene.ray_buf;
	scene->cl_data.scene.ray_buf = scene->cl_data.scene.normal_buf;
	scene->cl_data.scene.normal_buf = swap_pointer;
	get_closest_points(scene, 0, 1);
	get_intersection_buf(scene);
	get_normal_buf(scene);
	//clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, scene->cl_data.scene.prev_material_buf, 0, 0, sizeof(t_material) * WID * HEI, 0, NULL, NULL);
	get_material_buf(scene, 1);
	get_frame_buf(scene, 1);
	clFinish(scene->cl_data.commands);
}

void	get_reflection_ray(t_scene *scene)
{
	size_t global = WID * HEI;
	size_t local;
	cl_mem swap;
	cl_mem swap2;
	cl_mem swap_pointer = scene->cl_data.scene.ray_buf;
	swap2 = scene->cl_data.scene.normal_buf;
 	scene->cl_data.scene.ray_buf = scene->cl_data.scene.copy_normal_buf;
	scene->cl_data.scene.normal_buf = swap_pointer;
	scene->cl_data.scene.copy_normal_buf = swap2;
 	swap = scene->cl_data.scene.intersection_buf;
	scene->cl_data.scene.intersection_buf = scene->cl_data.scene.copy_intersec_buf;
	scene->cl_data.scene.copy_intersec_buf = swap; 
	clSetKernelArg(scene->cl_data.kernels[16], 0, sizeof(cl_mem), &scene->cl_data.scene.ray_buf);
	clSetKernelArg(scene->cl_data.kernels[16], 1, sizeof(cl_mem), &scene->cl_data.scene.normal_buf);
	clGetKernelWorkGroupInfo(scene->cl_data.kernels[16], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[16], 1, NULL, &global, &local, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
}

void	get_fresnel_coeff(t_scene *scene)
{
	size_t global = WID * HEI;
	size_t local;
	
	clSetKernelArg(scene->cl_data.kernels[17], 0, sizeof(cl_mem), &scene->cl_data.scene.ray_buf);
	clSetKernelArg(scene->cl_data.kernels[17], 1, sizeof(cl_mem), &scene->cl_data.scene.normal_buf);
	clSetKernelArg(scene->cl_data.kernels[17], 2, sizeof(cl_mem), &scene->cl_data.scene.material_buf);
	clGetKernelWorkGroupInfo(scene->cl_data.kernels[17], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[17], 1, NULL, &global, &local, 0, NULL, NULL);
	clFinish(scene->cl_data.commands);
}

void	draw_scene(t_sdl *sdl, t_scene *scene)
{
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;
	float				**matrix;
	cl_mem 				swap_pointer;
	size_t global = WID * HEI;

	x = -1;
	y = -1;
	i = 0;
	scene->bounce_cnt = 0;
	scene->max_bounces = 1;
	scene->has_refraction = 0;
	scene->init_flag = 1;
	size_t local;
	get_rays_arr(scene);
	while (scene->bounce_cnt < scene->max_bounces)
	{
		get_closest_points(scene, 0, 0);
		scene->init_flag = 0;
		get_intersection_buf(scene);
		get_normal_buf(scene);		
		get_material_buf(scene, 0);
		if (scene->bounce_cnt == 0)
			clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, scene->cl_data.scene.orig_index_buf, 0, 0, sizeof(int) * global, 0, NULL, NULL);
		clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, scene->cl_data.scene.prev_material_buf, 0, 0, sizeof(t_material) * global, 0, NULL, NULL);
		clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.normal_buf, scene->cl_data.scene.copy_normal_buf, 0, 0, sizeof(cl_float3) * global, 0, NULL, NULL);
		clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.intersection_buf, scene->cl_data.scene.copy_intersec_buf, 0, 0, sizeof(cl_float3) * global, 0, NULL, NULL);
		get_frame_buf(scene, 0);
 		if (scene->max_bounces > 1)
		{
			if (scene->has_refraction && scene->bounce_cnt == 0)
			{
				printf("*************has refraction\n************");
				get_fresnel_coeff(scene);
				get_refraction_ray(scene);
			}
			get_reflection_ray(scene);
		}
		clFinish(scene->cl_data.commands);
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
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;

	x = -1;
	y = -1;
	i = 0;
	scene->bounce_cnt = 0;
	get_rays_arr(scene);
	clEnqueueCopyBuffer(scene->cl_data.commands, scene->cl_data.scene.ray_buf, scene->cl_data.scene.normal_buf, 0, 0, sizeof(cl_float3) * WID * HEI, 0, NULL, NULL);
	get_closest_points(scene, 0, 0);
	get_intersection_buf(scene);
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
	register int		x;
	register int		y;
	register int		i;
	int					j = 0;
	cl_int err;
	x = -1;
	y = -1;
	i = 0;
		scene->bounce_cnt = 0;
		get_rays_arr(scene);
		get_closest_points(scene, 0, 0);
		clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.depth_buf, CL_FALSE, 0, sizeof(float) * WID * HEI, scene->depth_buf, 0, NULL, NULL);
		clFinish(scene->cl_data.commands);
		xyi[1] = -1;
		while (++xyi[1] < HEI)
		{
			xyi[0] = -1;
			while (++xyi[0] < WID)
			{
				xyi[2] = xyi[1] * WID + xyi[0];
				color.red = scene->depth_buf[xyi[2]] > 255 ? \
					255 : scene->depth_buf[xyi[2]];
					color.green = color.red;
					color.blue = color.red;
				SDL_SetRenderDrawColor(sdl->renderer, color.red, color.green, color.blue, 255);
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
	size_t global = WID * HEI;
	
	scene->bounce_cnt = 0;
	get_rays_arr(scene);
	get_closest_points(scene, 0, 0);
	get_intersection_buf(scene);
	get_normal_buf(scene);
	get_material_buf(scene, 0);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.index_buf, CL_FALSE, 0, sizeof(int) * (WID * HEI), scene->index_buf, 0, NULL, NULL);
	clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, CL_FALSE, 0, sizeof(t_material) * (WID * HEI), scene->material_buf, 0, NULL, NULL);
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

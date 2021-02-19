/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npetrell <npetrell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/08 17:32:12 by pmetron           #+#    #+#             */
/*   Updated: 2021/02/19 10:21:37 by npetrell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void		clear_default(t_object *obj)
{
	free(obj->data);
	free(obj);
}

static void	cl_release(t_scene *scene)
{
	ft_memdel((void **)&scene->normal_buf);
	ft_memdel((void **)&scene->material_buf);
	ft_memdel((void **)&scene->intersection_buf);
	ft_memdel((void **)&scene->ray_buf);
	ft_memdel((void **)&scene->viewport);
	ft_memdel((void **)&scene->index_buf);
	ft_memdel((void **)&scene->depth_buf);
	clReleaseMemObject(scene->cl_data.scene.ray_buf);
	clReleaseMemObject(scene->cl_data.scene.intersection_buf);
	clReleaseMemObject(scene->cl_data.scene.index_buf);
	clReleaseMemObject(scene->cl_data.scene.depth_buf);
	clReleaseMemObject(scene->cl_data.scene.normal_buf);
	clReleaseMemObject(scene->cl_data.scene.frame_buf);
	clReleaseMemObject(scene->cl_data.scene.copy_normal_buf);
	clReleaseMemObject(scene->cl_data.scene.prev_material_buf);
	clReleaseMemObject(scene->cl_data.scene.frame_buf_refl);
	clReleaseMemObject(scene->cl_data.scene.frame_buf_refr);
	clReleaseMemObject(scene->cl_data.scene.copy_intersec_buf);
	clReleaseMemObject(scene->cl_data.scene.orig_index_buf);
	clReleaseMemObject(scene->cl_data.scene.exception_buf);
	clReleaseMemObject(scene->cl_data.scene.obj);
}

void		clean_scene(t_scene *scene)
{
	int		i;

	i = -1;
	cl_release(scene);
	while (++i < 19)
	{
		clReleaseProgram(scene->cl_data.programs[i]);
		clReleaseKernel(scene->cl_data.kernels[i]);
	}
	clReleaseCommandQueue(scene->cl_data.commands);
	clReleaseContext(scene->cl_data.context);
	ft_memdel((void **)&scene->cl_data.kernels);
	ft_memdel((void **)&scene->cl_data.programs);
	while (++i < scene->obj_nmb)
		scene->objs[i]->clear_obj(scene->objs[i]);
	free(scene->light);
	free(scene->objs);
	ft_memdel((void **)&scene);
}

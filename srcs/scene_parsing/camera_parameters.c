/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_parameters.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/09 12:00:26 by hunnamab          #+#    #+#             */
/*   Updated: 2021/01/18 20:01:25 by pmetron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	get_camera(char **description, t_scene *scene)
{
	t_camera	camera;
	cl_float3	buf;

	camera.position = get_points(description[1]);
	buf = get_points(description[2]);
	camera.rotation[0] = buf.x;
	camera.rotation[1] = buf.y;
	camera.rotation[2] = buf.z;
	scene->camera = camera;
	printf("camera (%f,%f,%f)\n", camera.rotation[0],camera.rotation[1],camera.rotation[2]);
}
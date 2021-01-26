/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define_object.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/10 13:54:29 by hunnamab          #+#    #+#             */
/*   Updated: 2021/01/12 20:04:18 by pmetron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	get_parameters(char *name, char **description, t_scene *scene, int *snmi)
{
	t_object	*obj;

	obj = NULL;
	if (ft_strequ(name, "\t\"sphere\":")) //printf("sphere\n");
		get_sphere(description, scene, snmi); // objects_parameters.c
	else if (ft_strequ(name, "\t\"triangle\":")) //printf("triangle\n");
		get_triangle(description, 0.0, scene, snmi);
	else if (ft_strequ(name, "\t\"plane\":")) //printf("plane\n");
		get_plane(description,  scene, snmi);
	else if (ft_strequ(name, "\t\"cylinder\":")) //printf("cylinder\n");
		get_cylinder(description,  scene, snmi);
	else if (ft_strequ(name, "\t\"cone\":")) //printf("cone\n");
		get_cone(description,  scene, snmi);
	else
		output_error(4);
}

void	one_argument_triangle(char **description, t_scene *scene, int *snmi, float specular)
{
	t_object	*triangle;
	cl_float3	vertex[3];
	t_color		color;
	cl_float3	buf;
	float		rotation[3];

	vertex[0] = get_points(description[1]);
	vertex[1] = get_points(description[2]);
	vertex[2] = get_points(description[3]);
	buf = get_points(description[4]);
	rotation[0] = buf.x;
	rotation[1] = buf.y;
	rotation[2] = buf.z;
	color = get_color(description[5]);
	specular = ftoi(get_coordinates(description[6]));
	triangle = new_triangle(vertex, specular, color, rotation);
	triangle->text = tex_new_bmp(get_file(description[7]));
	scene->objs[snmi[1]] = triangle;
	snmi[1]++;
}

t_object 	*multiple_triangles(char **description, int *snmi, int i, float specular)
{
	t_object	*triangle;
	cl_float3	vertex[3];
	t_color		color;
	cl_float3	buf;
	float		rotation[3];
	
	vertex[0] = get_points(description[i + 1]);
	vertex[1] = get_points(description[i + 2]);
	vertex[2] = get_points(description[i + 3]);
	buf = get_points(description[i + 4]);
	rotation[0] = buf.x;
	rotation[1] = buf.y;
	rotation[2] = buf.z;
	color = get_color(description[i + 5]);
	specular = ftoi(get_coordinates(description[i + 6]));
	triangle = new_triangle(vertex, specular, color, rotation);
	return (triangle);
}
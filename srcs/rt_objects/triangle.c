/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldeirdre <ldeirdre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 21:42:26 by pmetron           #+#    #+#             */
/*   Updated: 2021/01/24 14:44:27 by ldeirdre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include "types.h"

static void	init_norme(float *r, cl_float3 *v, float *rt, cl_float3 *vt)
{
	rt[0] = r[0];
	rt[1] = r[1];
	rt[2] = r[2];
	vt[0] = v[0];
	vt[1] = v[1];
	vt[2] = v[2];
}

cl_float3		normalize_tr(cl_float3 vec)
{
	float length;

	length = (float)sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	return ((cl_float3){{vec.x / length, vec.y / length, vec.z / length}});
}

cl_float3		cross(cl_float3 u, cl_float3 v)
{
	return ((cl_float3){{u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x}});
}

t_basis			get_basis_tr(t_basis basis, cl_float3 *vert)
{
	cl_float3	v1;
	cl_float3	v2;
	cl_float3	normal;

	v1.x = vert[1].x - vert[0].x;
	v1.y = vert[1].y - vert[0].y;
	v1.z = vert[1].z - vert[0].z;
	v2.x = vert[2].x - vert[1].x;
	v2.y = vert[2].y - vert[1].y;
	v2.z = vert[2].z - vert[1].z;
	normal = normalize_tr(cross(v1, v2));
	basis.u = normal;
	basis.w = v1;
	basis.v = cross(basis.u, basis.w);
	return (basis);
}

t_object	*new_triangle(cl_float3 *vertex, float specular, t_color color, \
							float *rotation)
{
	t_triangle	*new_triangle;
	t_object	*new_object;
	cl_float3		edge1;
	cl_float3		edge2;
	float		**matrix;

	new_object = protected_malloc(sizeof(t_object), 1);
	new_triangle = protected_malloc(sizeof(t_triangle), 1);
	//new_triangle->vertex = protected_malloc(sizeof(cl_float3), 3);
	init_norme(rotation, vertex, new_object->rotation, new_triangle->vertex);
	matrix = get_rotation_matrix(new_object->rotation);
	transform(new_triangle->vertex, matrix, 3);
	matr_free(matrix, 4);
	edge1 = vector_sub(&new_triangle->vertex[0], &new_triangle->vertex[1]);
	edge2 = vector_sub(&new_triangle->vertex[1], &new_triangle->vertex[2]);
	new_triangle->normal = vector_cross(&edge1, &edge2);
	normalize_vector(&new_triangle->normal);
	new_object->specular = specular;
	new_object->text = NULL;
	new_object->color = color;
	new_object->data = (void *)new_triangle;
	new_object->tag = "triangle";
	new_object->type = TRIANGLE;
	new_object->intersect = &intersect_ray_triangle;
	new_object->get_normal = &get_triangle_normal;
	new_object->clear_obj = &clear_triangle;
	new_object->basis = get_basis_tr(new_object->basis, new_triangle->vertex);
	new_object->basis = get_basis(new_object->basis, new_object->rotation);
	return (new_object);
}

void		clear_triangle(t_object *obj)
{
	t_triangle *t;

	t = (t_triangle *)obj->data;
	free(t->vertex);
	free(obj->data);
	free(obj);
}

void		get_triangle_normal(t_scene *scene, int index, int obj_num)
{
	t_triangle *t;

	t = (t_triangle *)scene->objs[obj_num]->data;
	copy_point(&scene->normal_buf[index], &t->normal);
	if (vector_dot(&scene->ray_buf[index], \
	&scene->normal_buf[index]) > 0.0001)
		scene->normal_buf[index] = vector_scale(&scene->normal_buf[index], -1);
}

/* float		intersect_ray_triangle(t_scene *scene, int index, cl_float3 *start, cl_float3 *dir)
{
	t_triangle	*triangle;
	cl_float3	edge[2];
	cl_float3	vec[3];
	float		det;
	float		uv[2];

	triangle = (t_triangle *)scene->objs[index]->data;
	edge[0] = vector_sub(&triangle->vertex[1], &triangle->vertex[0]);
	edge[1] = vector_sub(&triangle->vertex[2], &triangle->vertex[0]);
	vec[0] = vector_cross(dir, &edge[1]);
	det = vector_dot(&edge[0], &vec[0]);
	if (det < 1e-8 && det > -1e-8)
		return (0);
	det = 1 / det;
	vec[1] = vector_sub(start, &triangle->vertex[0]);
	uv[0] = vector_dot(&vec[1], &vec[0]) * det;
	if (uv[0] < 0 || uv[0] > 1)
		return (0);
	vec[2] = vector_cross(&vec[1], &edge[0]);
	uv[1] = vector_dot(dir, &vec[2]) * det;
	if (uv[1] < 0 || uv[0] + uv[1] > 1)
		return (0);
	if ((vector_dot(&edge[1], &vec[2]) * det) > 0)
		return (vector_dot(&edge[1], &vec[2]) * det);
	return (0);
} */

void		intersect_ray_triangle(t_scene *scene, int index)
{
	size_t global = WID * HEI;

	size_t local;

	//t_triangle *tri = (t_triangle *)scene->objs[index]->data;

	clSetKernelArg(scene->cl_data.kernels[4], 0, sizeof(cl_mem), &scene->cl_data.scene.normal_buf);
	clSetKernelArg(scene->cl_data.kernels[4], 1, sizeof(cl_mem), &scene->cl_data.scene.intersection_buf);
	clSetKernelArg(scene->cl_data.kernels[4], 2, sizeof(cl_mem), &scene->cl_data.scene.depth_buf);
	clSetKernelArg(scene->cl_data.kernels[4], 3, sizeof(t_triangle), scene->objs[index]->data);
	clSetKernelArg(scene->cl_data.kernels[4], 4, sizeof(cl_mem), &scene->cl_data.scene.index_buf);
	clSetKernelArg(scene->cl_data.kernels[4], 5, sizeof(cl_int), (void*)&index);

    clGetKernelWorkGroupInfo(scene->cl_data.kernels[4], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	printf("local == max work group size == %ld\n", local);
    clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[4], 1, NULL, &global, &local, 0, NULL, NULL);
}
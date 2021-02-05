#include "rt.h"

void	one_argument_torus(char **description, t_scene *scene, int *snmi)
{
	t_object	*torus;
	cl_float3	cen_buf[3];
	float		specular[5];
	t_color		color;
	int surface_id;

	cen_buf[0] = get_points(description[1]);
	cen_buf[1] = get_points(description[2]);
	specular[0] = ftoi(get_coordinates(description[3]));
	specular[1] = ftoi(get_coordinates(description[4]));
	cen_buf[2] = get_points(description[5]);
	color = get_color(description[6]);
	specular[2] = ftoi(get_coordinates(description[7]));
	specular[3] = ftoi(get_coordinates(description[8]));
	specular[4] = ftoi(get_coordinates(description[9]));
	surface_id = ftoi(get_coordinates(description[10]));
	torus = new_torus(cen_buf, color, specular, surface_id);
	torus->text = tex_new_bmp(get_file(description[11]));
	scene->objs[snmi[1]] = torus;
	snmi[1]++;
}

t_object 	*multiple_torus(char **description, t_scene *scene, int *snmi, int i)
{
	t_object	*torus;
	cl_float3	cen_buf[3];
	float		specular[5];
	t_color		color;
	int surface_id;

	cen_buf[0] = get_points(description[i + 1]);
	cen_buf[1] = get_points(description[i + 2]);
	specular[0] = ftoi(get_coordinates(description[i + 3]));
	specular[1] = ftoi(get_coordinates(description[i + 4]));
	cen_buf[2] = get_points(description[i + 5]);
	color = get_color(description[i + 6]);
	specular[2] = ftoi(get_coordinates(description[i + 7]));
	specular[3] = ftoi(get_coordinates(description[i + 8]));
	specular[4] = ftoi(get_coordinates(description[i + 9]));
	surface_id = ftoi(get_coordinates(description[i + 10]));
	torus = new_torus(cen_buf, color, specular, surface_id);
	return (torus);
}

void	get_torus(char **description, t_scene *scene, int *snmi)
{
	t_object	*torus;
	int i;

	i = 1;
	//printf("center %c\n", description[0][0]);
	if (description[0][0] == '[')
	{
		while (description[i][1] != ']')
		{
			//printf("text %c\n", description[i][2]);
			if (description[i][2] == '{')
			{
				torus = multiple_torus(description, scene, snmi, i);
				torus->text = tex_new_bmp(get_file(description[i + 11]));
				scene->objs[snmi[1]] = torus;
				snmi[1]++;
				i += 13;
			}
		}
	}
	if (description[0][0] == '{')
		one_argument_torus(description, scene, snmi);
}

t_object    *new_torus(cl_float3 *cen_buf, t_color color, float *specular, int surface_id)
{
    t_torus	*torus;
	t_object		*new_object;
	float			**matrix;

	new_object = malloc(sizeof(t_object));
	torus = malloc(sizeof(t_torus));
	torus->center = cen_buf[0]; 
	torus->vec = cen_buf[1]; 
	torus->radius1 = specular[0];
	torus->radius2 = specular[1];
	new_object->rotation[0] = cen_buf[2].x;
	new_object->rotation[1] = cen_buf[2].y;
	new_object->rotation[2] = cen_buf[2].z;
	new_object->specular = specular[2];
	new_object->reflection = specular[3];
	new_object->refraction = specular[4];
	new_object->color = color;
	new_object->text = NULL;
	new_object->normal_text = NULL;
	new_object->data = (void *)torus;
	new_object->type = TORUS;
	new_object->cs_nmb = 0;
	new_object->surface_id = surface_id;
	new_object->cutting_surfaces = NULL;
	new_object->intersect = &intersect_ray_torus;
	//new_object->get_normal = &get_torus_normal;
	new_object->clear_obj = &clear_default;
	return (new_object);
}

void        intersect_ray_torus(t_scene *scene, int index)
{
    size_t global = WID * HEI;
	size_t local;
	cl_int err = 0;
	t_torus *t;
	t = scene->objs[index]->data;
	printf("torus radius host (%f,%f)\n", t->radius1, t->radius2);
    err = clSetKernelArg(scene->cl_data.kernels[13], 0, sizeof(cl_mem), &scene->cl_data.scene.normal_buf);
	printf("%d", err);
	err = clSetKernelArg(scene->cl_data.kernels[13], 1, sizeof(cl_mem), &scene->cl_data.scene.intersection_buf);
	printf("%d", err);
    err = clSetKernelArg(scene->cl_data.kernels[13], 2, sizeof(t_torus), scene->objs[index]->data);
	printf("%d", err);
	err = clSetKernelArg(scene->cl_data.kernels[13], 3, sizeof(cl_mem), &scene->cl_data.scene.depth_buf);
	printf("%d", err);
	err = clSetKernelArg(scene->cl_data.kernels[13], 4, sizeof(cl_mem), &scene->cl_data.scene.index_buf);
	printf("%d", err);
	err = clSetKernelArg(scene->cl_data.kernels[13], 5, sizeof(cl_int), (void*)&index);
	printf("%d", err);
	clSetKernelArg(scene->cl_data.kernels[13], 6, sizeof(cl_int), (void*)&scene->bounce_cnt);
	clSetKernelArg(scene->cl_data.kernels[13], 7, sizeof(cl_mem), &scene->cl_data.scene.material_buf);
    clGetKernelWorkGroupInfo(scene->cl_data.kernels[13], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	printf("torus local == %ld\n", local);
    err = clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[13], 1, NULL, &global, &local, 0, NULL, NULL);
	printf("%d\n", err);
}

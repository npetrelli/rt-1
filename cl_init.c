#include "rt.h"

int    cl_init(t_scene *scene)
{
	int err;
	unsigned int correct; // number of correct results returned
	unsigned int count = WID * HEI;
	size_t global; // global domain size for our calculation
	size_t local; // local domain size for our calculation
	int i;
	err = 0;
    cl_uint nP;
    cl_uint status = clGetPlatformIDs(0, NULL, &nP);
    cl_platform_id pfs;
    status = clGetPlatformIDs(nP, &pfs, NULL);
    size_t size;
    char *str;
    clGetPlatformInfo(pfs, CL_PLATFORM_NAME, 0, NULL, &size);
    str = malloc(sizeof(char) * size);
    clGetPlatformInfo(pfs, CL_PLATFORM_NAME, size, str, NULL);
    printf("%s\n", str);
	err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &scene->cl_data.device_id, NULL);

	// выделение памяти
	scene->cl_data.programs = malloc(sizeof(cl_program) * KERNEL_NUM);
	scene->cl_data.kernels = malloc(sizeof(cl_kernel) * KERNEL_NUM);

	int		ret1;
	char	*get_ray_arr;
	int fd1 = open("./kernels/get_ray_arr.cl", O_RDONLY);
	get_ray_arr = protected_malloc(sizeof(char), 256000);
	ret1 = read(fd1, get_ray_arr, 64000);
	get_ray_arr[ret1] = '\0';

	scene->cl_data.context = clCreateContext(0, 1, &scene->cl_data.device_id, NULL, NULL, &err);
	scene->cl_data.commands = clCreateCommandQueue(scene->cl_data.context, scene->cl_data.device_id, 0, &err);
	if ((scene->cl_data.programs[0] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&get_ray_arr, NULL, &err)))
		printf("created\n");
	if ((clBuildProgram(scene->cl_data.programs[0], 0, NULL, NULL, NULL, &err)))
		printf("built\n");
	if (!(scene->cl_data.kernels[0] = clCreateKernel(scene->cl_data.programs[0], "get_ray_arr", &err)))
		printf("error %d\n", err);
	close(fd1);

	char info[1024];
	clGetDeviceInfo(scene->cl_data.device_id, CL_DEVICE_NAME, 1024, info, NULL);
	printf("%s\n", info);
	
	int		ret2;
	char	*intersect_ray_sphere_cl;
	int fd2 = open("./kernels/intersect_ray_sphere_cl.cl", O_RDONLY);
	intersect_ray_sphere_cl = protected_malloc(sizeof(char), 256000);
	ret2 = read(fd2, intersect_ray_sphere_cl, 64000);
	intersect_ray_sphere_cl[ret2] = '\0';
	
	if ((scene->cl_data.programs[1] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&intersect_ray_sphere_cl, NULL, &err)))
		printf("cоздана программа sphere\n");
	if ((clBuildProgram(scene->cl_data.programs[1], 0, NULL, NULL, NULL, &err)))
		printf("собрана программа sphere\n");
	if (!(scene->cl_data.kernels[1] = clCreateKernel(scene->cl_data.programs[1], "intersect_ray_sphere_cl", &err)))
		printf("не собрана программа 1, error %d sphere\n", err);
	close(fd2);

	int		ret3;
	char	*intersect_ray_cone_cl;
	int fd3 = open("./kernels/intersect_ray_cone_cl.cl", O_RDONLY);
	intersect_ray_cone_cl = protected_malloc(sizeof(char), 256000);
	ret3 = read(fd3, intersect_ray_cone_cl, 64000);
	intersect_ray_cone_cl[ret3] = '\0';
	
	if ((scene->cl_data.programs[2] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&intersect_ray_cone_cl, NULL, &err)))
		printf("cоздана программа cone\n");
	if ((clBuildProgram(scene->cl_data.programs[2], 0, NULL, NULL, NULL, &err)))
		printf("собрана программа cone\n");
	if (!(scene->cl_data.kernels[2] = clCreateKernel(scene->cl_data.programs[2], "intersect_ray_cone_cl", &err)))
		printf("не собрана программа 1, error %d cone\n", err);
	close(fd3);

	int		ret4;
	char	*intersect_ray_cylinder_cl;
	int fd4 = open("./kernels/intersect_ray_cylinder_cl.cl", O_RDONLY);
	intersect_ray_cylinder_cl = protected_malloc(sizeof(char), 256000);
	ret4 = read(fd4, intersect_ray_cylinder_cl, 64000);
	intersect_ray_cylinder_cl[ret4] = '\0';
	
	if ((scene->cl_data.programs[3] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&intersect_ray_cylinder_cl, NULL, &err)))
		printf("cоздана программа cylinder\n");
	if ((clBuildProgram(scene->cl_data.programs[3], 0, NULL, NULL, NULL, &err)))
		printf("собрана программа cylinder\n");
	if (!(scene->cl_data.kernels[3] = clCreateKernel(scene->cl_data.programs[3], "intersect_ray_cylinder_cl", &err)))
		printf("не собрана программа 1, error %d cylinder\n", err);
	close(fd4);

	int		ret5;
	char	*intersect_ray_triangle_cl;
	int fd5 = open("./kernels/intersect_ray_triangle_cl.cl", O_RDONLY);
	intersect_ray_triangle_cl = protected_malloc(sizeof(char), 256000);
	ret5 = read(fd5, intersect_ray_triangle_cl, 64000);
	intersect_ray_triangle_cl[ret5] = '\0';
	
	if ((scene->cl_data.programs[4] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&intersect_ray_triangle_cl, NULL, &err)))
		printf("cоздана программа triangle\n");
	if ((clBuildProgram(scene->cl_data.programs[4], 0, NULL, NULL, NULL, &err)))
		printf("собрана программа triangle\n");
	if (!(scene->cl_data.kernels[4] = clCreateKernel(scene->cl_data.programs[4], "intersect_ray_triangle_cl", &err)))
		printf("не собрана программа 1, error %d triangle\n", err);
	close(fd5);

	int		ret6;
	char	*intersect_ray_plane_cl;
	int fd6 = open("./kernels/intersect_ray_plane_cl.cl", O_RDONLY);
	intersect_ray_plane_cl = protected_malloc(sizeof(char), 256000);
	ret6 = read(fd6, intersect_ray_plane_cl, 64000);
	intersect_ray_plane_cl[ret6] = '\0';
	
	if ((scene->cl_data.programs[5] = clCreateProgramWithSource(scene->cl_data.context, 1, (const char **)&intersect_ray_plane_cl, NULL, &err)))
		printf("cоздана программа plane\n");
	if ((clBuildProgram(scene->cl_data.programs[5], 0, NULL, NULL, NULL, &err)))
		printf("собрана программа plane\n");
	if (!(scene->cl_data.kernels[5] = clCreateKernel(scene->cl_data.programs[5], "intersect_ray_plane_cl", &err)))
		printf("не собрана программа 1, error %d plane\n", err);
	close(fd6);

	//Создание буферов на гпу
	scene->cl_data.scene.ray_buf = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(cl_float3) * count, NULL, NULL);
	scene->cl_data.scene.viewport = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(cl_float3) * count, NULL, NULL);
	scene->cl_data.scene.camera = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(cl_float3), NULL, NULL);
	scene->cl_data.scene.intersection_buf = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(cl_float3) * count, NULL, NULL);
	scene->cl_data.scene.index_buf = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(int) * count, NULL, NULL);
	scene->cl_data.scene.depth_buf = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(float) * count, NULL, NULL);
	scene->cl_data.scene.normal_buf = clCreateBuffer(scene->cl_data.context,  CL_MEM_READ_WRITE,  sizeof(cl_float3) * count, NULL, NULL);
	return (0);
}

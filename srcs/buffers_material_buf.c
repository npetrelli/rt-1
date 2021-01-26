#include "rt.h"
#include "types.h"

void	get_material_buf(t_scene *scene)
{
	size_t global;
	size_t local;

	global = WID * HEI;
    clSetKernelArg(scene->cl_data.kernels[10], 0, sizeof(cl_mem), &scene->cl_data.scene.textures);
	clSetKernelArg(scene->cl_data.kernels[10], 1, sizeof(cl_mem), &scene->cl_data.scene.obj);
    clSetKernelArg(scene->cl_data.kernels[10], 2, sizeof(cl_mem), &scene->cl_data.scene.index_buf);
	clSetKernelArg(scene->cl_data.kernels[10], 3, sizeof(cl_mem), &scene->cl_data.scene.intersection_buf);
	clSetKernelArg(scene->cl_data.kernels[10], 4, sizeof(cl_mem), &scene->cl_data.scene.material_buf);
    clGetKernelWorkGroupInfo(scene->cl_data.kernels[10], scene->cl_data.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	printf("local get material buf == %ld\n", local);
    clEnqueueNDRangeKernel(scene->cl_data.commands, scene->cl_data.kernels[10], 1, NULL, &global, &local, 0, NULL, NULL);
    clFinish(scene->cl_data.commands);
	//clEnqueueReadBuffer(scene->cl_data.commands, scene->cl_data.scene.material_buf, CL_FALSE, 0, sizeof(t_material) * global, scene->material_buf, 0, NULL, NULL);
	printf("material_color default color (%hhu,%hhu,%hhu)\n", scene->material_buf[1280 * 360 - 640].color.red,\
	scene->material_buf[1280 * 360 - 640].color.green,scene->material_buf[1280 * 360 - 640].color.blue);
	printf("depthbuf %f\n", scene->depth_buf[1280 * 360 - 640]);
	printf("sizeof(t_material) host %lu\n", sizeof(t_material));
}
/*
 * main.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include <iostream>
#include <fstream>
#include <string>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"
#define TAILLE 1024

int main(int argc, char *argv[])
{
  CLManager clm;
  cl_platform_id *platforms = NULL;
  cl_device_id *devices = NULL;
  cl_device_id device_id = NULL;
  cl_int ret = 0;
  cl_uint num_devices;
  cl_uint num_platforms;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;

  clm.printPlatform();
  
  // TODO parser la saisie utilisateur (sur quel numero de device on travaille et le fichier d'input)
  


  // test
  int i;
  int *A = (int*)malloc(sizeof(int)*TAILLE);
  int *B = (int*)malloc(sizeof(int)*TAILLE);
  for(int i = 0; i < TAILLE; i++) {
    A[i] = i;
    B[i] = TAILLE - i;
  }
  
  // charge le kernel dans une string
  std::ifstream ifs("bin/test_addition.cl");
  std::string contenu_str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  const char *source_str = contenu_str.c_str();
  size_t source_size = contenu_str.size();
  
  // recup les infos plateforme/devices
  clm.getAllPlatforms(&num_platforms, &platforms);
  clm.getAllDevicesByPlatform(platforms[0], &num_devices, &devices);
  device_id = devices[1];

  // initialisation du contexte et de la file de commandes
  clm.init(device_id, &context, &command_queue);
 
  // alloc des parametres 
  cl_mem a_mem_obj = clCreateBuffer(context,
				    CL_MEM_READ_ONLY, 
				    TAILLE * sizeof(int),
				    NULL,
				    &ret);
  cl_mem b_mem_obj = clCreateBuffer(context,
				    CL_MEM_READ_ONLY,
				    TAILLE * sizeof(int),
				    NULL,
				    &ret);
  cl_mem c_mem_obj = clCreateBuffer(context,
				    CL_MEM_WRITE_ONLY, 
				    TAILLE * sizeof(int),
				    NULL,
				    &ret);
 
  // copie des params dans la file de commandes
  ret = clEnqueueWriteBuffer(command_queue,
			     a_mem_obj,
			     CL_TRUE,
			     0,
			     TAILLE * sizeof(int),
			     A,
			     0,
			     NULL,
			     NULL);
  ret = clEnqueueWriteBuffer(command_queue,
			     b_mem_obj,
			     CL_TRUE,
			     0, 
			     TAILLE * sizeof(int),
			     B,
			     0,
			     NULL,
			     NULL);
 
  // creation du kernel
  cl_program program = clCreateProgramWithSource(context,
						 1,
						 &source_str,
						 &source_size, &ret);
  clm.check(ret, "creation du programme");
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  clm.check(ret, "build du programme");
  cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
  clm.check(ret, "creation du kernel");
 
  // initialisation du kernel avec ses parametres
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
  ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
  ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
 
  // execution du kernel
  size_t global_item_size = TAILLE;
  size_t local_item_size = 64; // jdivise en paquets de 64
  ret = clEnqueueNDRangeKernel(command_queue,
			       kernel,
			       1,
			       NULL,
			       &global_item_size,
			       &local_item_size,
			       0,
			       NULL,
			       NULL);
  clm.check(ret, "execution du kernel");
 
  // lecture du resultat
  int *C = (int*)malloc(sizeof(int)*TAILLE);
  ret = clEnqueueReadBuffer(command_queue,
			    c_mem_obj,
			    CL_TRUE,
			    0, 
			    4 * sizeof(int),
			    C,
			    0,
			    NULL,
			    NULL);
 
  // verif du calcul
  for(i = 0; i < 4; i++)
    printf("%d + %d = %d\n", A[i], B[i], C[i]);
 
  // clean/free
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(a_mem_obj);
  ret = clReleaseMemObject(b_mem_obj);
  ret = clReleaseMemObject(c_mem_obj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
  free(A);
  free(B);
  free(C);

  
  return 0;
}



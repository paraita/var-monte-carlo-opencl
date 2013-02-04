/*
 * test.h
 *
 *  Created on: 16 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <streambuf>
#include <algorithm>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"
#define PRINT_USAGE "Usage: -s S0 -t horizon -v volatilite"
#define TAILLE 1024
#define KERNEL_EXEMPLE_2 "__kernel void toto(__global int *INPUT, \
                                             __global int *OUTPUT, \
                                             __global int *incr, \
    					     __global int *nb) { \
                                      int i = get_global_id(0); \
				      for (int j = 0; j < *nb; j++) { \
				        OUTPUT[i] += *incr; \
                                      } \
			  }"
#define KERNEL_PROTO_TEST "__kernel void prototype(__global const float *PORTEFEUILLE, \
                                                   __global const float *ALEA, \
                                                   __global float *TIRAGES, \
                                                   __global int *nb_actions, \
    					           __global int *horizon) { \
                                      int i = get_global_id(0); \
                                      TIRAGES[i] = PORTEFEUILLE[0]; \
			   }"



void parse_args(int argc, char** argv, float* s0, float* horizon, float* volat)
{  
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)) {
    CLManager clm;
    std::cout << PRINT_USAGE << std::endl;
    std::cout << "Plateformes disponibles:" << std::endl;
    clm.printPlatform();
    exit(0);
  }
  
  else {
    if (argc != 7) {
      std::cout << PRINT_USAGE << std::endl;
      exit(0);
    }
    else {
      bool s0_ok = false;
      bool horizon_ok = false;
      bool volat_ok = false;
      for (int i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-s") == 0) {
	  if (i == argc-1 || s0_ok) {
	    std::cout << PRINT_USAGE << std::endl;
	    exit(0);
	  }
	  else {
	    *s0 = atof(argv[++i]);
	    s0_ok = true;
	    continue;
	  }
	}
	if (strcmp(argv[i], "-t") == 0) {
	  if (i == argc-1 || horizon_ok) {
	    std::cout << PRINT_USAGE << std::endl;
	    exit(0);
	  }
	  else {
	    *horizon = atof(argv[++i]);
	    horizon_ok = true;
	    continue;
	  }
	}
	if (strcmp(argv[i], "-v") == 0) {
	  if (i == argc-1 || volat_ok) {
	    std::cout << PRINT_USAGE << std::endl;
	    exit(0);
	  }
	  else {
	    *volat = atof(argv[++i]);
	    volat_ok = true;
	    continue;
	  }
	}
      }
      if (!s0_ok || !horizon_ok || !volat_ok) {
	std::cout << PRINT_USAGE << std::endl;
	exit(0);
      }
    }
  }
}

void exemple_addition()
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
}

void exemple_2(int argc, char *argv[])
{
  int incr = 0;
  int nb = 0;
  if (argv[1]) incr = atoi(argv[1]);
  if (argv[2]) nb = atoi(argv[2]);
  std::cout << "incr=" << incr << std::endl;
  std::cout << "nb threads et taille tableau=" << nb << std::endl;
  if (incr == 0 && nb == 0) return; // sale
  int *INPUT;
  int *OUTPUT;
  INPUT = (int *) calloc(nb, sizeof(int));
  OUTPUT = (int *) calloc(nb, sizeof(int));
  std::string source = KERNEL_EXEMPLE_2;
  const char *source_str = source.c_str();
  size_t source_size = source.size();
  CLManager clm;
  cl_int ret;
  cl_platform_id *platforms = NULL;
  cl_device_id *devices = NULL;
  cl_device_id device_id = NULL;
  cl_uint num_devices;
  cl_uint num_platforms;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  clm.getAllPlatforms(&num_platforms, &platforms);
  clm.getAllDevicesByPlatform(platforms[0], &num_devices, &devices);
  device_id = devices[1];
  clm.init(device_id, &context, &command_queue);
  cl_mem mem_input = clCreateBuffer(context,
			      CL_MEM_READ_ONLY,
			      nb * sizeof(int),
			      NULL,
			      &ret);
  clm.check(ret, "clCreateBuffer mem_input");
  cl_mem mem_output = clCreateBuffer(context,
			       CL_MEM_WRITE_ONLY,
			       nb * sizeof(int),
			       NULL,
			       &ret);
  clm.check(ret, "clCreateBuffer mem_output");
  cl_mem mem_nb = clCreateBuffer(context,
			   CL_MEM_READ_ONLY,
			   sizeof(int),
			   NULL,
			   &ret);
  clm.check(ret, "clCreateBuffer mem_nb");
  cl_mem mem_incr = clCreateBuffer(context,
			     CL_MEM_READ_ONLY,
			     sizeof(int),
			     NULL,
			     &ret);
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_input,
			     CL_TRUE,
			     0,
			     nb * sizeof(int),
			     INPUT,
			     0,
			     NULL,
			     NULL);
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_incr,
			     CL_TRUE,
			     0,
			     sizeof(int),
			     &incr,
			     0,
			     NULL,
			     NULL);
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_nb,
			     CL_TRUE,
			     0,
			     sizeof(int),
			     &nb,
			     0,
			     NULL,
			     NULL);
  cl_program program = clCreateProgramWithSource(context,
						 1,
						 &source_str,
						 &source_size,
						 &ret);
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  cl_kernel kernel = clCreateKernel(program, "toto", &ret);
  clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_input);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_output);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mem_incr);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&mem_nb);
  size_t global_item_size = nb;
  size_t local_item_size = 32;
  ret = clEnqueueNDRangeKernel(command_queue,
			       kernel,
			       1,
			       NULL,
			       &global_item_size,
			       &local_item_size,
			       0,
			       NULL,
			       NULL);
  ret = clEnqueueReadBuffer(command_queue,
			    mem_output,
			    CL_TRUE,
			    0,
			    nb * sizeof(int),
			    OUTPUT,
			    0,
			    NULL,
			    NULL);
  for (int i = 0; i < nb; i++) {
    std::cout << "OUTPUT[" << i << "]=" <<OUTPUT[i] << std::endl;
  }
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(mem_output);
  ret = clReleaseMemObject(mem_input);
  ret = clReleaseMemObject(mem_nb);
  ret = clReleaseMemObject(mem_incr);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
  free(INPUT);
  free(OUTPUT);	       
}

void prototype()
{
  const float seuil_confiance = 0.99;
  int NB_ACTIONS = 1;
  int NB_TIRAGES = 100000;
  int T = 1;
  float *N = (float *) calloc(NB_ACTIONS * NB_TIRAGES * T, sizeof(float));
  float *TIRAGES = (float *) calloc(NB_TIRAGES, sizeof(float));
  std::ifstream t("kernels/prototype.cl");
  std::string source((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  const char *source_str = source.c_str();
  std::cout << "source: " << source << std::endl;
  size_t source_size = source.size();
  CLManager clm;
  cl_int ret;
  cl_platform_id *platforms = NULL;
  cl_device_id *devices = NULL;
  cl_device_id device_id = NULL;
  cl_uint num_devices;
  cl_uint num_platforms;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;

  clm.printPlatform();
  std::cout << "parametres:" << std::endl;
  std::cout << "\tnb actions: " << NB_ACTIONS << std::endl;
  std::cout << "\tnb tirages: " << NB_TIRAGES << std::endl;
  std::cout << "\thorizon: " << T << std::endl;
  
  // portefeuille
  float P[NB_ACTIONS];
  P[0] = 123.0;
  P[1] = 99.0;
  P[2] = 100.0;
  P[3] = 54.0;
  P[4] = 18.0;
  P[5] = 6.0;
  P[6] = 13.0;
  P[7] = 67.0;
  P[8] = 589.0;
  P[9] = 64.0;

  // taille des valeurs aleatoires dans la global
  std::cout << "\tOccupation mémoire (toutes les gaussiennes): ";
  std::cout << NB_ACTIONS * NB_TIRAGES * T * sizeof(float) / 1024 / 1024;
  std::cout << " Mo" << std::endl;

  // print portefeuille
  std::cout << "Portefeuille:" << std::endl;
  float rendement_portefeuille = 0;
  for(int g = 0; g < NB_ACTIONS; g++) {
    printf("\tP[%d]=%f\n", g, P[g]);
    rendement_portefeuille += P[g];
  }
  
  // je remplit le tableau avec toutes les gaussiennes
  boost::mt19937 rng;
  boost::normal_distribution<> nd(0.0, 1.0);
  boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
  for(int g = 0; g < NB_ACTIONS * NB_TIRAGES * T; g++) {
    N[g] = var_nor();
  }

  // recup du device GPU
  clm.getAllPlatforms(&num_platforms, &platforms);
  clm.getAllDevicesByPlatform(platforms[0], &num_devices, &devices);
  device_id = devices[1];
  clm.init(device_id, &context, &command_queue);

  // j'alloc les parametres du kernel
  cl_mem mem_P = clCreateBuffer(context,
				CL_MEM_READ_ONLY,
				NB_ACTIONS * sizeof(float),
				NULL,
				&ret);
  clm.check(ret, "createbuffer mem_P");
  cl_mem mem_N = clCreateBuffer(context,
				CL_MEM_READ_ONLY,
				NB_ACTIONS * NB_TIRAGES * T * sizeof(float),
				NULL,
				&ret);
  clm.check(ret, "createbuffer mem_N");
  cl_mem mem_TIRAGES = clCreateBuffer(context,
				CL_MEM_WRITE_ONLY,
				NB_TIRAGES * sizeof(float),
				NULL,
				&ret);
  clm.check(ret, "createbuffer mem_TIRAGES");
  cl_mem mem_nb_actions = clCreateBuffer(context,
				CL_MEM_READ_ONLY,
				sizeof(float),
				NULL,
				&ret);
  clm.check(ret, "createbuffer mem_nb_actions");
  cl_mem mem_horizon = clCreateBuffer(context,
				CL_MEM_READ_ONLY,
				sizeof(float),
				NULL,
				&ret);
  clm.check(ret, "createbuffer mem_horizon");

  ret = clEnqueueWriteBuffer(command_queue,
			     mem_P,
			     CL_TRUE,
			     0,
			     NB_ACTIONS * sizeof(float),
			     P,
			     0,
			     NULL,
			     NULL);
  clm.check(ret, "enqueuewritebuffer mem_P");
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_N,
			     CL_TRUE,
			     0,
			     NB_ACTIONS * NB_TIRAGES * T * sizeof(float),
			     N,
			     0,
			     NULL,
			     NULL);
  clm.check(ret, "enqueuewritebuffer mem_N");
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_nb_actions,
			     CL_TRUE,
			     0,
			     sizeof(float),
			     &NB_ACTIONS,
			     0,
			     NULL,
			     NULL);
  clm.check(ret, "enqueuewritebuffer mem_nb_actions");
  ret = clEnqueueWriteBuffer(command_queue,
			     mem_horizon,
			     CL_TRUE,
			     0,
			     sizeof(float),
			     &T,
			     0,
			     NULL,
			     NULL);
  clm.check(ret, "enqueuewritebuffer mem_horizon");
  
  // je charge mon kernel
  cl_program program = clCreateProgramWithSource(context,
						 1,
						 &source_str,
						 &source_size,
						 &ret);
  clm.check(ret, "createprogramwithsource");

  // je compile le kernel
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  cl_kernel kernel = clCreateKernel(program, "prototype", &ret);
  clm.check(ret, "createkernel");

  // je prepare l'appel avec les bons arguments
  clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_P);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_N);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mem_TIRAGES);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&mem_nb_actions);
  clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&mem_horizon);

  // j'execute le kernel
  size_t global_item_size = NB_TIRAGES;
  size_t local_item_size = 1; // shared
  ret = clEnqueueNDRangeKernel(command_queue,
			       kernel,
			       1,
			       NULL,
			       &global_item_size,
			       &local_item_size,
			       0,
			       NULL,
			       NULL);

  // je chope le tableau des tirages resultant
  ret = clEnqueueReadBuffer(command_queue,
			    mem_TIRAGES,
			    CL_TRUE,
			    0,
			    NB_TIRAGES * sizeof(float),
			    TIRAGES,
			    0,
			    NULL,
			    NULL);
  clm.check(ret, "enqueuereedbuffer");

  // je trie le tableau des tirages
  // std::cout << "TIRAGES[] apres recuperation des resultats:" << std::endl;
  // for(int g = 0; g < NB_TIRAGES; g++) {
  //   printf("\tTIRAGES[%d]=%f\n", g, TIRAGES[g]);
  // }
  std::sort(TIRAGES, TIRAGES+NB_TIRAGES);
  // std::cout << "TIRAGES[] apres tri:" << std::endl;
  // for(int g = 0; g < NB_TIRAGES; g++) {
  //   printf("\tTIRAGES[%d]=%f\n", g, TIRAGES[g]);
  // }
  std::ofstream fd;
  fd.open("tirages.data");
  for(int g = 0; g < NB_TIRAGES; g++) {
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
  
  // VaR gain
  int percentile = NB_TIRAGES * int(1.0 - seuil_confiance);
  std::cout << "Valeur du portefeuille aujourd'hui: " << rendement_portefeuille << std::endl;
  std::cout << "la VaR(" << T << "," << (seuil_confiance * 100);
  std::cout << "%) est de " << TIRAGES[percentile+1] << std::endl;
  
  // je clean
  free(N);
  free(TIRAGES);
}

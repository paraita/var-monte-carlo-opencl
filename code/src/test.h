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


void prototype()
{
  /* const float seuil_confiance = 0.99; */
  /* int NB_ACTIONS = 1; */
  /* int NB_TIRAGES = 100000; */
  /* int T = 1; */
  /* float *N = (float *) calloc(NB_ACTIONS * NB_TIRAGES * T, sizeof(float)); */
  /* float *TIRAGES = (float *) calloc(NB_TIRAGES, sizeof(float)); */
  /* std::ifstream t("kernels/prototype.cl"); */
  /* std::string source((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()); */
  /* const char *source_str = source.c_str(); */
  /* std::cout << "source: " << source << std::endl; */
  /* size_t source_size = source.size(); */
  /* CLManager clm; */
  /* cl_int ret; */
  /* cl_platform_id *platforms = NULL; */
  /* cl_device_id *devices = NULL; */
  /* cl_device_id device_id = NULL; */
  /* cl_uint num_devices; */
  /* cl_uint num_platforms; */
  /* cl_context context = NULL; */
  /* cl_command_queue command_queue = NULL; */

  /* clm.printPlatform(); */
  /* std::cout << "parametres:" << std::endl; */
  /* std::cout << "\tnb actions: " << NB_ACTIONS << std::endl; */
  /* std::cout << "\tnb tirages: " << NB_TIRAGES << std::endl; */
  /* std::cout << "\thorizon: " << T << std::endl; */
  
  /* // portefeuille */
  /* float P[NB_ACTIONS]; */
  /* P[0] = 123.0; */
  /* P[1] = 99.0; */
  /* P[2] = 100.0; */
  /* P[3] = 54.0; */
  /* P[4] = 18.0; */
  /* P[5] = 6.0; */
  /* P[6] = 13.0; */
  /* P[7] = 67.0; */
  /* P[8] = 589.0; */
  /* P[9] = 64.0; */

  /* // taille des valeurs aleatoires dans la global */
  /* std::cout << "\tOccupation mémoire (toutes les gaussiennes): "; */
  /* std::cout << NB_ACTIONS * NB_TIRAGES * T * sizeof(float) / 1024 / 1024; */
  /* std::cout << " Mo" << std::endl; */

  /* // print portefeuille */
  /* std::cout << "Portefeuille:" << std::endl; */
  /* float rendement_portefeuille = 0; */
  /* for(int g = 0; g < NB_ACTIONS; g++) { */
  /*   printf("\tP[%d]=%f\n", g, P[g]); */
  /*   rendement_portefeuille += P[g]; */
  /* } */
  
  /* // je remplit le tableau avec toutes les gaussiennes */
  /* boost::mt19937 rng; */
  /* boost::normal_distribution<> nd(0.0, 1.0); */
  /* boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd); */
  /* for(int g = 0; g < NB_ACTIONS * NB_TIRAGES * T; g++) { */
  /*   N[g] = var_nor(); */
  /* } */

  /* // recup du device GPU */
  /* clm.getAllPlatforms(&num_platforms, &platforms); */
  /* clm.getAllDevicesByPlatform(platforms[0], &num_devices, &devices); */
  /* device_id = devices[1]; */
  /* clm.init(device_id, &context, &command_queue); */

  /* // j'alloc les parametres du kernel */
  /* cl_mem mem_P = clCreateBuffer(context, */
  /* 				CL_MEM_READ_ONLY, */
  /* 				NB_ACTIONS * sizeof(float), */
  /* 				NULL, */
  /* 				&ret); */
  /* clm.check(ret, "createbuffer mem_P"); */
  /* cl_mem mem_N = clCreateBuffer(context, */
  /* 				CL_MEM_READ_ONLY, */
  /* 				NB_ACTIONS * NB_TIRAGES * T * sizeof(float), */
  /* 				NULL, */
  /* 				&ret); */
  /* clm.check(ret, "createbuffer mem_N"); */
  /* cl_mem mem_TIRAGES = clCreateBuffer(context, */
  /* 				CL_MEM_WRITE_ONLY, */
  /* 				NB_TIRAGES * sizeof(float), */
  /* 				NULL, */
  /* 				&ret); */
  /* clm.check(ret, "createbuffer mem_TIRAGES"); */
  /* cl_mem mem_nb_actions = clCreateBuffer(context, */
  /* 				CL_MEM_READ_ONLY, */
  /* 				sizeof(float), */
  /* 				NULL, */
  /* 				&ret); */
  /* clm.check(ret, "createbuffer mem_nb_actions"); */
  /* cl_mem mem_horizon = clCreateBuffer(context, */
  /* 				CL_MEM_READ_ONLY, */
  /* 				sizeof(float), */
  /* 				NULL, */
  /* 				&ret); */
  /* clm.check(ret, "createbuffer mem_horizon"); */

  /* ret = clEnqueueWriteBuffer(command_queue, */
  /* 			     mem_P, */
  /* 			     CL_TRUE, */
  /* 			     0, */
  /* 			     NB_ACTIONS * sizeof(float), */
  /* 			     P, */
  /* 			     0, */
  /* 			     NULL, */
  /* 			     NULL); */
  /* clm.check(ret, "enqueuewritebuffer mem_P"); */
  /* ret = clEnqueueWriteBuffer(command_queue, */
  /* 			     mem_N, */
  /* 			     CL_TRUE, */
  /* 			     0, */
  /* 			     NB_ACTIONS * NB_TIRAGES * T * sizeof(float), */
  /* 			     N, */
  /* 			     0, */
  /* 			     NULL, */
  /* 			     NULL); */
  /* clm.check(ret, "enqueuewritebuffer mem_N"); */
  /* ret = clEnqueueWriteBuffer(command_queue, */
  /* 			     mem_nb_actions, */
  /* 			     CL_TRUE, */
  /* 			     0, */
  /* 			     sizeof(float), */
  /* 			     &NB_ACTIONS, */
  /* 			     0, */
  /* 			     NULL, */
  /* 			     NULL); */
  /* clm.check(ret, "enqueuewritebuffer mem_nb_actions"); */
  /* ret = clEnqueueWriteBuffer(command_queue, */
  /* 			     mem_horizon, */
  /* 			     CL_TRUE, */
  /* 			     0, */
  /* 			     sizeof(float), */
  /* 			     &T, */
  /* 			     0, */
  /* 			     NULL, */
  /* 			     NULL); */
  /* clm.check(ret, "enqueuewritebuffer mem_horizon"); */
  
  /* // je charge mon kernel */
  /* cl_program program = clCreateProgramWithSource(context, */
  /* 						 1, */
  /* 						 &source_str, */
  /* 						 &source_size, */
  /* 						 &ret); */
  /* clm.check(ret, "createprogramwithsource"); */

  /* // je compile le kernel */
  /* ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL); */
  /* cl_kernel kernel = clCreateKernel(program, "prototype", &ret); */
  /* clm.check(ret, "createkernel"); */

  /* // je prepare l'appel avec les bons arguments */
  /* clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_P); */
  /* clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_N); */
  /* clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&mem_TIRAGES); */
  /* clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&mem_nb_actions); */
  /* clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&mem_horizon); */

  /* // j'execute le kernel */
  /* size_t global_item_size = NB_TIRAGES; */
  /* size_t local_item_size = 1; // shared */
  /* ret = clEnqueueNDRangeKernel(command_queue, */
  /* 			       kernel, */
  /* 			       1, */
  /* 			       NULL, */
  /* 			       &global_item_size, */
  /* 			       &local_item_size, */
  /* 			       0, */
  /* 			       NULL, */
  /* 			       NULL); */

  /* // je chope le tableau des tirages resultant */
  /* ret = clEnqueueReadBuffer(command_queue, */
  /* 			    mem_TIRAGES, */
  /* 			    CL_TRUE, */
  /* 			    0, */
  /* 			    NB_TIRAGES * sizeof(float), */
  /* 			    TIRAGES, */
  /* 			    0, */
  /* 			    NULL, */
  /* 			    NULL); */
  /* clm.check(ret, "enqueuereedbuffer"); */

  /* // je trie le tableau des tirages */
  /* // std::cout << "TIRAGES[] apres recuperation des resultats:" << std::endl; */
  /* // for(int g = 0; g < NB_TIRAGES; g++) { */
  /* //   printf("\tTIRAGES[%d]=%f\n", g, TIRAGES[g]); */
  /* // } */
  /* std::sort(TIRAGES, TIRAGES+NB_TIRAGES); */
  /* // std::cout << "TIRAGES[] apres tri:" << std::endl; */
  /* // for(int g = 0; g < NB_TIRAGES; g++) { */
  /* //   printf("\tTIRAGES[%d]=%f\n", g, TIRAGES[g]); */
  /* // } */
  /* std::ofstream fd; */
  /* fd.open("tirages.data"); */
  /* for(int g = 0; g < NB_TIRAGES; g++) { */
  /*   fd << TIRAGES[g] << std::endl; */
  /* } */
  /* fd.close(); */
  
  /* // VaR gain */
  /* int percentile = NB_TIRAGES * int(1.0 - seuil_confiance); */
  /* std::cout << "Valeur du portefeuille aujourd'hui: " << rendement_portefeuille << std::endl; */
  /* std::cout << "la VaR(" << T << "," << (seuil_confiance * 100); */
  /* std::cout << "%) est de " << TIRAGES[percentile+1] << std::endl; */
  
  /* // je clean */
  /* free(N); */
  /* free(TIRAGES); */
}

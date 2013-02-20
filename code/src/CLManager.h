/*
 * CLManager.h
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#ifndef CLMANAGER_H_
#define CLMANAGER_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class CLManager {

 public:
  CLManager(); // ok
  ~CLManager(); // ok
  std::string printPlatform(); // ok
  void loadKernels(const char*); // ok
  void init(const int, const int); // ok
  void compileKernel(const std::string); // ok
  void setKernelArg(const std::string,
		    const unsigned int,
		    const int,
		    const size_t,
		    void *,
		    const bool); // ok
  void executeKernel(const int, const std::string); // ok
  void getResultat(); // ok
  void check(const cl_int, const std::string); // ok
  void reset(); // ok
 private:
  void * resultat;
  size_t size_resultat;
  std::map< std::string, cl_kernel > kernels;
  std::vector<cl_platform_id> platforms;
  std::vector< std::vector<cl_device_id> > devices;
  std::vector<cl_mem> buff_mems;
  unsigned int device_no;
  unsigned int platform_no;
  unsigned int output_buff_mem_pos;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  void cleanCL(); // ok
  void err_check(const cl_int, const std::string, const bool); // ok
};


#endif /* CLMANAGER_H_ */

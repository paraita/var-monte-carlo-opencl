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
#define ENABLE_PROFILING true
#define DISABLE_PROFILING false
typedef bool profiling_status;

class CLManager {

 public:
  CLManager(); // ok
  ~CLManager(); // ok
  std::string printPlatform(); // ok
  void loadKernels(const char*); // ok
  void init(const int, const int); // ok
  void init(const int, const int, profiling_status debug); // ok
  void compileKernel(const std::string); // ok
  void setKernelArg(const std::string,
		    const unsigned int,
		    const int,
		    const size_t,
		    void *,
		    const bool); // ok
  void executeKernel(const int, const std::string); // ok
  void getResultat(); // ok
  float getGpuTime(); // ok
  void check(const cl_int, const std::string); // ok
  void reset(); // ok
 private:
  std::map< std::string, cl_kernel > kernels; // ok
  std::vector<cl_platform_id> platforms; // ok
  std::vector< std::vector<cl_device_id> > devices; // ok
  std::vector<cl_mem> buff_mems;
  void * resultat;
  size_t size_resultat;
  unsigned int device_no;
  unsigned int platform_no;
  unsigned int output_buff_mem_pos;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  profiling_status debug_mode;
  cl_event prof_event;
  cl_ulong ev_start_time;
  cl_ulong ev_end_time;
  int maxWorkGroupSize;
  void cleanCL(); // ok
  void err_check(const cl_int, const std::string, const bool); // ok
};


#endif /* CLMANAGER_H_ */

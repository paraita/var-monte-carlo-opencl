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
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


class CLManager {

 public:
  CLManager();
  std::string printPlatform();
  void loadKernel(const std::string, const char **, size_t *);
  void init(const int platform, const int device);
  void clean();
  cl_program compileKernel();
  void setKernelArg();
  void executeKernel();
  void check(const cl_int, const std::string);
 private:
  cl_device_id titoi;
  std::vector<cl_platform_id> platforms;
  std::vector< std::vector<cl_device_id> > devices;
  cl_context context;
  cl_command_queue command_queue;
  void err_check(const cl_int, const std::string, const bool);
};


#endif /* CLMANAGER_H_ */

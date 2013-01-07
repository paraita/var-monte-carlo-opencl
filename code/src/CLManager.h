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
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


class CLManager {

 public:
  void printPlatform();
  void loadKernel(const std::string, const char **, size_t *);
  void init(const cl_device_id, cl_context *, cl_command_queue *);
  void clean();
  cl_program compileKernel();
  void setKernelArg();
  void executeKernel();
  void getAllPlatforms(cl_uint *, cl_platform_id **);
  void getAllDevicesByPlatform(cl_platform_id, cl_uint *, cl_device_id **);
  void check(const cl_int, const std::string);
 private:
  void err_check(const cl_int, const std::string, const bool);
};


#endif /* CLMANAGER_H_ */

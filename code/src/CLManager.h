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
  cl_context init();
  void clean();
  cl_program compileKernel();
  void setKernelArg();
  void executeKernel();
 private:
  void err_check(const cl_int, const std::string, const bool);

};


#endif /* CLMANAGER_H_ */

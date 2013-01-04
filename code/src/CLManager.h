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
#include <CL/cl.h>

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

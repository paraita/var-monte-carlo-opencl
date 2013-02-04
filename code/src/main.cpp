/*
 * main.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include <iostream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <string>
#include <string.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"
#include "test.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#define PRINT_USAGE "Usage: -s S0 -t horizon -v volatilite"

void parse_args(int argc, char** argv, float* s0, float* horizon, float* volat);
void exemple_addition();
void exemple_2(int argc, char *argv[]);
void prototype();

int main(int argc, char *argv[])
{

  //exemple_addition();
  
  //exemple_2(argc, argv);

  prototype();
  
  return 0;
}















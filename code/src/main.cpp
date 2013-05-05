/*
 * main.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

//#include <iostream>
//#include <fstream>
//#include <streambuf>
//#include <algorithm>
//#include <string>
//#include <string.h>
//#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "Portefeuille.h"
#include "CLManager.h"
#include "utils.h"
#include "tests.h"
#define PRINT_USAGE "Usage: -c seuil_confiance -n nb_tirages -p portefeuille -t horizon [-b]"


int main(int argc, char *argv[])
{
  bool param_ok = false;
  float seuil_confiance = 0;
  int nb_tirages = 0;
  std::string portefeuille;
  int horizon = 0;
  bool batch_mode = false;
  
  // verif des parametres
  param_ok = parse_args(argc,
			argv,
			&seuil_confiance,
			&nb_tirages,
			&portefeuille,
			&horizon,
			&batch_mode);
  
  if (param_ok) {
    calcul2(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    return EXIT_SUCCESS;
  }
  else {
    std::cout << PRINT_USAGE << std::endl;
    return EXIT_FAILURE;
  }
}















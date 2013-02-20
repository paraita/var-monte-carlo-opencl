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
void prototype();

void calcul() {
  
  CLManager clm;
  std::cout << clm.printPlatform();
  clm.init(0,0);

  clm.loadKernels("kernels/prototype.cl");
  clm.compileKernel("prototype");

  const float seuil_confiance = 0.99;
  int NB_ACTIONS = 10;
  int NB_TIRAGES = 100;
  int T = 1;
  float *N = (float *) calloc(NB_ACTIONS * NB_TIRAGES * T, sizeof(float));
  float *TIRAGES = (float *) calloc(NB_TIRAGES, sizeof(float));

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

  // print portefeuille
  std::cout << "Portefeuille:" << std::endl;
  float rendement_portefeuille = 0;
  for(int g = 0; g < NB_ACTIONS; g++) {
    printf("\tP[%d]=%f\n", g, P[g]);
    rendement_portefeuille += P[g];
  }
  
  // ~~~~~~~~~~~~~~~~~~~~~~ RNG ~~~~~~~~~~~~~~~~~~~~~~~
  boost::mt19937 rng;
  boost::normal_distribution<> nd(0.0, 1.0);
  boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
  for(int g = 0; g < NB_ACTIONS * NB_TIRAGES * T; g++) {
    N[g] = var_nor();
  }
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~~~~~~~~ params ~~~~~~~~~~~~~~~~~~~~~
  clm.setKernelArg("prototype",0,NB_ACTIONS,sizeof(float),P,false);
  clm.setKernelArg("prototype", 1, NB_ACTIONS * NB_TIRAGES * T, sizeof(float), N, false);
  clm.setKernelArg("prototype", 2, NB_TIRAGES, sizeof(float), TIRAGES, true);
  clm.setKernelArg("prototype", 3, 1, sizeof(int), &NB_ACTIONS, false);
  clm.setKernelArg("prototype", 4, 1, sizeof(int), &T, false);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // run sur le GPU
  clm.executeKernel(NB_TIRAGES, "prototype");

  // recuperation des résultats
  clm.getResultat();

  // ~~~~~~~~~~~~~~ post-traitement VaR ~~~~~~~~~~~~~~~
  std::sort(TIRAGES, TIRAGES+NB_TIRAGES);
  // graph
  std::ofstream fd;
  fd.open("tirages.data");
  for(int g = 0; g < NB_TIRAGES; g++) {
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
  int percentile = NB_TIRAGES * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  std::cout << "Valeur du portefeuille aujourd’hui: " << rendement_portefeuille << std::endl;
  std::cout << "la VaR(" << T << "," << (seuil_confiance * 100);
  std::cout << "%) est de " << TIRAGES[percentile+1] << std::endl;
}

int main(int argc, char *argv[])
{

  //exemple_addition();
  
  //exemple_2(argc, argv);

  //prototype();
  
  calcul();
  
  return 0;
}















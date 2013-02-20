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
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#define PRINT_USAGE "Usage: -c seuil_confiance -n nb_tirages -p portefeuille -t horizon"


bool parse_args(int argc,char** argv,float* seuil_confiance,int* nb_tirages,int* p,int* horizon);
void calcul(float seuil_confiance,int nb_tirages,int portefeuille,int T);

int main(int argc, char *argv[])
{
  bool param_ok = false;
  float seuil_confiance = 0;
  int nb_tirages = 0;
  int portefeuille = 0;
  int horizon = 0;
  
  // verif des parametres
  param_ok = parse_args(argc, argv, &seuil_confiance, &nb_tirages, &portefeuille, &horizon);
  
  if (param_ok) {
    printf("Parametres:\n\tseuil de confiance:%f\n\tnb tirages:%d\n\thorizon:%d\n",
	   seuil_confiance,nb_tirages,horizon);
    calcul(seuil_confiance, nb_tirages, portefeuille, horizon);
    return EXIT_SUCCESS;
  }
  else {
    std::cout << PRINT_USAGE << std::endl;
    return EXIT_FAILURE;
  }
}

bool parse_args(int argc,
		char** argv,
		float* seuil_confiance,
		int* nb_tirages,
		int* p,
		int* horizon)
{
  // affiche l'aide et le hardware dispo sur la machine
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)) {
    CLManager clm;
    std::cout << "Plateformes disponibles:" << std::endl;
    std::cout << clm.printPlatform() << std::endl;
    return false;
  }
  else {
    if (argc != 9) {
      std::cout << "\tpas le bon nombre de params ! (" << argc << ")" << std::endl;
      return false;
    }
    // sinon on recupere les parametres
    else {
      bool _seuil_confiance_ok = false;
      bool _nb_tirages_ok = false;
      bool _p_ok = false;
      bool _horizon_ok = false;
      for (int i = 1; i < argc; i++) {
	// parametre: seuil de confiance
	if (strcmp(argv[i], "-c") == 0) {
	  if (i == argc-1 || _seuil_confiance_ok) {
	    std::cout << "\terreur seuil de confiance" << std::endl;
	    return false;
	  }
	  else {
	    *seuil_confiance = atof(argv[++i]);
	    _seuil_confiance_ok = true;
	    continue;
	  }
	}
	// parametre: nombre de tirages
	if (strcmp(argv[i], "-n") == 0) {
	  if (i == argc-1 || _nb_tirages_ok) {
	    std::cout << "\terreur nombre de tirages" << std::endl;
	    return false;
	  }
	  else {
	    *nb_tirages = atoi(argv[++i]);
	    _nb_tirages_ok = true;
	    continue;
	  }
	}
	// parametre: portefeuille
	if (strcmp(argv[i], "-p") == 0) {
	  if (i == argc-1 || _p_ok) {
	    std::cout << "\terreur portefeuille" << std::endl;
	    return false;
	  }
	  else {
	    *p = atoi(argv[++i]);
	    _p_ok = true;
	    continue;
	  }
	}
	// parametre: horizon
	if (strcmp(argv[i], "-t") == 0) {
	  if (i == argc-1 || _horizon_ok) {
	    std::cout << "\terreur horizon" << std::endl;
	    return false;
	  }
	  else {
	    *horizon = atoi(argv[++i]);
	    _horizon_ok = true;
	    continue;
	  }
	}
      }
      // est ce qu'on a tout les parametres ?
      if (!_seuil_confiance_ok ||
	  !_nb_tirages_ok ||
	  !_p_ok ||
	  !_horizon_ok) {
	std::cout << "\terreur on a pas tout les parametres" << std::endl;
	return false;
      }
      else {
	// est ce que les parametres sont valides ?
	if ( *seuil_confiance < 1 &&
	     *seuil_confiance > 0 &&
	     *nb_tirages > 0 &&
	     *horizon > 0) {
	  return true;
	}
	else {
	  std::cout << "\terreur parametre(s) non valide(s)" << std::endl;
	  return false;
	}
      }
    }
  }
}

void calcul(float seuil_confiance, int nb_tirages, int portefeuille, int T) {
  CLManager clm;
  std::cout << clm.printPlatform();
  clm.init(0,0);
  clm.loadKernels("kernels/prototype.cl");
  clm.compileKernel("prototype");
  
  int NB_ACTIONS = 10;
  float *N = (float *) calloc(NB_ACTIONS * nb_tirages * T, sizeof(float));
  float *TIRAGES = (float *) calloc(nb_tirages, sizeof(float));

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
  for(int g = 0; g < NB_ACTIONS * nb_tirages * T; g++) {
    N[g] = var_nor();
  }
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~~~~~~~~ params ~~~~~~~~~~~~~~~~~~~~~
  clm.setKernelArg("prototype",0,NB_ACTIONS,sizeof(float),P,false);
  clm.setKernelArg("prototype", 1, NB_ACTIONS * nb_tirages * T, sizeof(float), N, false);
  clm.setKernelArg("prototype", 2, nb_tirages, sizeof(float), TIRAGES, true); // sortie
  clm.setKernelArg("prototype", 3, 1, sizeof(int), &NB_ACTIONS, false);
  clm.setKernelArg("prototype", 4, 1, sizeof(int), &T, false);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // run sur le GPU
  clm.executeKernel(nb_tirages, "prototype");

  // recuperation des résultats
  clm.getResultat();

  // ~~~~~~~~~~~~~~ post-traitement VaR ~~~~~~~~~~~~~~~
  std::sort(TIRAGES, TIRAGES+nb_tirages);
  // graph
  std::ofstream fd;
  fd.open("tirages.data");
  for(int g = 0; g < nb_tirages; g++) {
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
  int percentile = nb_tirages * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  std::cout << "Valeur du portefeuille aujourd’hui: " << rendement_portefeuille << std::endl;
  std::cout << "la VaR(" << T << "," << (seuil_confiance * 100);
  std::cout << "%) est de " << TIRAGES[percentile+1] << std::endl;
}













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
#include "Portefeuille.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
//#include <boost/chrono.hpp>
#define PRINT_USAGE "Usage: -c seuil_confiance -n nb_tirages -p portefeuille -t horizon [-b]"


bool parse_args(int argc,char** argv,float* seuil_confiance,int* nb_tirages,std::string* p,int* horizon, bool* batch_mode);
// RNG sur CPU, calcul trajectoires sur GPU, VaR sur CPU
void calcul1(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);
// RNG sur GPU
void calcul2();

void calculVariance(	float *TIRAGES,
			int *nb_Simulation,
			int *nb_value_par_thread,
			float *esperance,
			float *variance,
			int *nb_THREAD,  
	      		float *intervalConfiance,      	 
			float *CARRE,
			float *ESPERANCE );


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
    //calcul1(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    calcul2();
    std::cout << "fin du programme" << std::endl;
    return EXIT_SUCCESS;
  }
  else {
    std::cout << PRINT_USAGE << std::endl;
    return EXIT_FAILURE;
  }
}


void calcul2() {
  CLManager clm;
  clm.init(0,1,ENABLE_PROFILING);
  int nb_tirages = 98304;
  int ul_nb_tirages = 98304;
  int offset = 0;
  int acc = 0;
  std::string nom_kernel("EstimatePi");
  clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, 1, sizeof(int), &ul_nb_tirages, false);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), &offset, false);
  clm.setKernelArg(nom_kernel, 2, 1, sizeof(int), &acc, true);
  clm.executeKernel(nb_tirages, nom_kernel);
  clm.getResultat();
  std::cout << "Nombre de tirages out: " << acc << std::endl;
}

void calculVariance(	float *TIRAGES,
			int *nb_Simulation,
			int *nb_value_par_thread,
			float *esperance,
			float *variance,
			int *nb_THREAD,  
	      		float *intervalConfiance,      	 
			float *CARRE,
			float *ESPERANCE )
{
  std::cout << "on attaque la variance"<<std::endl; 
 std::cout << "on a "<< *esperance << std::endl;
CLManager clm;
  std::string nom_kernel("calcul_variance");
  clm.init(0,1,ENABLE_PROFILING);
  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/outil.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, *nb_Simulation,sizeof(float), TIRAGES,false);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), nb_Simulation, false);
  clm.setKernelArg(nom_kernel, 2, 1, sizeof(int), nb_value_par_thread, false);
  clm.setKernelArg(nom_kernel, 3, 1, sizeof(float), esperance, true); // sortie
  clm.setKernelArg(nom_kernel, 4, 1, sizeof(float), variance, true);  // sortie
  clm.setKernelArg(nom_kernel, 5, 1, sizeof(int), nb_THREAD, false);
  clm.setKernelArg(nom_kernel, 6, 1, sizeof(float), intervalConfiance, false);
  clm.setKernelArg(nom_kernel, 7, *nb_THREAD,sizeof(float), CARRE,false);
  clm.setKernelArg(nom_kernel, 8, *nb_THREAD,sizeof(float), ESPERANCE,false);
  // run sur le GPU
  clm.executeKernel(*nb_Simulation, nom_kernel);
  // recuperation des résultats
  clm.getResultat();
  // on calcul la variance
  std::cout << "fin du calcul " << std::endl;
}

void calcul1(float seuil_confiance,
	    int nb_tirages,
	    std::string portefeuille,
	    int T,
      bool batch_mode) {
  Portefeuille P(portefeuille);
  float *RENDEMENTS = P.getRendements();
  float *VOLS = P.getVolatilites();
  float *TI = P.getTauxInterets();
  int NB_ACTIONS = P.getTaille();
  
  // debug
  if (batch_mode)
  {
    std::cout << "Mémoire utilisée:" << std::endl;
    int taille_rendements = sizeof(float) * NB_ACTIONS;
    std::cout << "\tRENDEMENTS: " << taille_rendements << " octets" << std::endl;
    int taille_vols = sizeof(float) * NB_ACTIONS;
    std::cout << "\tVOLS: " << taille_vols << " octets" << std::endl;
    int taille_ti = sizeof(float) * NB_ACTIONS;
    std::cout << "\tTI: " << taille_ti << " octets" << std::endl;
    int taille_n = sizeof(float) * NB_ACTIONS * T  * nb_tirages;
    std::cout << "\tN: " << taille_n << " octets" << std::endl;
    int taille_nb_actions = sizeof(int);
    std::cout << "\tvariable nb_actions: " << taille_nb_actions << " octets" << std::endl;
    int taille_horizon = sizeof(int);
    std::cout << "\tvariable horizon: " << taille_horizon << " octets" << std::endl;
    int total = taille_rendements + taille_vols + taille_ti + taille_n + taille_nb_actions + taille_horizon;
    std::cout << "\tTOTAL: " << total / 1000000.0 << " Mo" << std::endl;
  }

  // ~~~~~~~~~~~~~~~~~~~~~~ RNG ~~~~~~~~~~~~~~~~~~~~~~~
 // boost::chrono::high_resolution_clock::time_point start_rng = boost::chrono::high_resolution_clock::now();
  float *N = (float *) calloc(NB_ACTIONS * nb_tirages * T, sizeof(float));
  float *TIRAGES = (float *) calloc(nb_tirages, sizeof(float));
  boost::mt19937 rng;
  boost::normal_distribution<> nd(0.0, 1.0);
  boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
  for(int g = 0; g < NB_ACTIONS * nb_tirages * T; g++) {
    N[g] = var_nor();
  }
 // boost::chrono::nanoseconds ns_rng = boost::chrono::high_resolution_clock::now() - start_rng;
  // ~~~~~~~~~~~~~~~~~~~~~ OpenCL ~~~~~~~~~~~~~~~~~~~~~
  CLManager clm;
  std::string nom_kernel("calcul_trajectoires");
  clm.init(0,1,ENABLE_PROFILING);
  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/var-mc.cl");
  //clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, NB_ACTIONS, sizeof(float), RENDEMENTS,false);
  clm.setKernelArg(nom_kernel, 1, NB_ACTIONS, sizeof(float), VOLS, false);
  clm.setKernelArg(nom_kernel, 2, NB_ACTIONS, sizeof(float), TI, false);
  clm.setKernelArg(nom_kernel, 3, NB_ACTIONS * nb_tirages * T, sizeof(float), N, false);
  clm.setKernelArg(nom_kernel, 4, nb_tirages, sizeof(float), TIRAGES, true); // sortie
  clm.setKernelArg(nom_kernel, 5, 1, sizeof(int), &NB_ACTIONS, false);
  clm.setKernelArg(nom_kernel, 6, 1, sizeof(int), &T, false);
  // run sur le GPU
  clm.executeKernel(nb_tirages, nom_kernel);
  // recuperation des résultats
  clm.getResultat();
  // on calcul la variance 
  int  nombre_TIRAGES_par_Thread = (nb_tirages) / (nb_tirages);
  float esperance=0;
  float variance=0;
  int   nb_THREAD = nb_tirages;		
  float  intervalConfiance = 0.01;
  float *CARRE = (float *) calloc(nb_tirages, sizeof(float));
  float *ESPERANCE = (float *) calloc(nb_tirages, sizeof(float));
  calculVariance(TIRAGES,&nb_tirages,&nombre_TIRAGES_par_Thread,&esperance,&variance,&nb_THREAD,&intervalConfiance,CARRE,ESPERANCE);
  // fin calcul de variance 

  // ~~~~~~~~~~~~~~ post-traitement VaR ~~~~~~~~~~~~~~~
 // boost::chrono::high_resolution_clock::time_point start_sort = boost::chrono::high_resolution_clock::now();
  std::sort(TIRAGES, TIRAGES+nb_tirages);
 // boost::chrono::nanoseconds ns_sort = boost::chrono::high_resolution_clock::now() - start_sort;
  
  int percentile = nb_tirages * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 // float t_rng = ns_rng.count() / 1000000.0;
 // float t_sort = ns_sort.count() / 1000000.0;
  std::cout << nb_tirages << ";";
  std::cout << P.getRendement() << ";";
  std::cout << TIRAGES[percentile+1] << ";";
  std::cout << NB_ACTIONS * nb_tirages * T * sizeof(float) << ";";
 // std::cout << t_rng << ";";
  std::cout << clm.getGpuTime() << ";";
 // std::cout << t_sort << std::endl;
}

bool parse_args(int argc,
		char** argv,
		float* seuil_confiance,
		int* nb_tirages,
		std::string* p,
		int* horizon,
		bool* batch_mode)
{
  // affiche l'aide et le hardware dispo sur la machine
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)) {
    CLManager clm;
    clm.init(0,0);
    std::cout << "Plateformes disponibles:" << std::endl;
    std::cout << clm.printPlatform() << std::endl;
    return false;
  }
  else {
    if (argc < 9 || argc > 10) {
      std::cout << "\tpas le bon nombre de params ! (" << argc << ")" << std::endl;
      return false;
    }
    // sinon on recupere les parametres
    else {
      bool _seuil_confiance_ok = false;
      bool _nb_tirages_ok = false;
      bool _p_ok = false;
      bool _horizon_ok = false;
      bool _batch_mode_ok = false;
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
	    *p = argv[++i];
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
	// parametre: batch mode
	if (strcmp(argv[i], "-b") == 0) {
	  if (_batch_mode_ok) {
	    std::cout << "\terreur batch mode" << std::endl;
	    return false;
	  }
	  else {
	    i = i + 1;
	    *batch_mode = true;
	    _batch_mode_ok = true;
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









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
#include <boost/chrono.hpp>
#define PRINT_USAGE "Usage: -c seuil_confiance -n nb_tirages -p portefeuille -t horizon [-b]"

// parsing des parametres d'entree
bool parse_args(int argc,char** argv,float* seuil_confiance,int* nb_tirages,std::string* p,int* horizon, bool* batch_mode);
// RNG sur CPU, calcul trajectoires sur GPU, tri sur CPU
void calcul1(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);
// RNG sur GPU, calcul trajectoires sur GPU, tri sur CPU
void calcul2(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);
// calcul 1 + variance et interval de confiance
void calcul5(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);
// calcul de variance
float calculVariance(float *TIRAGES,int *nb_Simulation,int *nb_value_par_thread,float esperance,int *nb_THREAD,float *ESPERANCE );
// calcul de l'espérance
float calculEsperance(float *TIRAGES,int *nb_Simulation,int *nb_value_par_thread,int *nb_THREAD,float *ESPERANCE);
// estimation de PI par MC sur GPU
void estimationPi(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);
// distribution gaussienne
void distributionGaussienne(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug);

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

    //calcul5(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    //calcul1(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    calcul2(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    //distributionGaussienne(seuil_confiance,nb_tirages,portefeuille,horizon,batch_mode);
    return EXIT_SUCCESS;
  }
  else {
    std::cout << PRINT_USAGE << std::endl;
    return EXIT_FAILURE;
  }
}

float calculEsperance(	float *TIRAGES,
			int *nb_Simulation,
			int *nb_value_par_thread,
			int *nb_THREAD, 
			float *ESPERANCE )
{
  float esp=(float)10.0;
  float  *esperance =&esp;
  CLManager clm;
  std::string nom_kernel("calcul_esperance");
  clm.init(0,1,ENABLE_PROFILING);
  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/esperance.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, *nb_Simulation,sizeof(float), TIRAGES,false);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), nb_Simulation, false);
  clm.setKernelArg(nom_kernel, 2, 1, sizeof(int), nb_value_par_thread, false);
  clm.setKernelArg(nom_kernel, 3, 1, sizeof(float), esperance, true); // sortie
  clm.setKernelArg(nom_kernel, 4, 1, sizeof(int), nb_THREAD, false);
  clm.setKernelArg(nom_kernel, 5, *nb_THREAD,sizeof(float), ESPERANCE,false); 
  // clm.setKernelArg(nom_kernel,0,1,sizeof(float),esperance,true);
  // run sur le GPU
  clm.executeKernel(*nb_Simulation, nom_kernel);
  // recuperation des résultats
  clm.getResultat();
  // on a l'esperance, maintenant la variance !
  //  for(int i=0; i< (*nb_Simulation);i++){ if( i > 0.99999 *(*nb_Simulation)){std::cout << ESPERANCE[i] << " ";}} 
  std::cout << " esperance "<< *esperance << std::endl;
  return *esperance;
}

float calculVariance(	float *TIRAGES,
			int *nb_Simulation,
			int *nb_value_par_thread,
			float esperance,
			int *nb_THREAD,  
			float *ESPERANCE )
{
  float var=(float)10.0;
  float *variance=&var;
  CLManager clm;
  std::string nom_kernel("calcul_variance");
  clm.init(0,1,ENABLE_PROFILING);
  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/variance.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, *nb_Simulation,sizeof(int), TIRAGES,false);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), nb_Simulation, false);
  clm.setKernelArg(nom_kernel, 2, 1, sizeof(int), nb_value_par_thread, false);
  clm.setKernelArg(nom_kernel, 3, 1, sizeof(float),&esperance, false);                                                      
  clm.setKernelArg(nom_kernel, 4, 1, sizeof(float),variance, true);  // sortie                                                            
  clm.setKernelArg(nom_kernel, 5, 1, sizeof(int), nb_THREAD, false);
  clm.setKernelArg(nom_kernel, 6, *nb_THREAD,sizeof(float), ESPERANCE,false);                                                      
  // run sur le GPU                                                                                                                          
  clm.executeKernel(*nb_Simulation, nom_kernel);
  // recuperation des résultats                                                                                                              
  clm.getResultat();
  std::cout << " variance " << *variance << std::endl;
  // on calcul la variance

  //  for(int j=0; j < (*nb_THREAD);j++){if(j> 0.99999*(*nb_THREAD))std::cout << ESPERANCE[j]<< " ";}
  
  std::cout << "interval de confiance " << 1.96*sqrt(*variance)/(sqrt(*nb_Simulation)) << std::endl ; 
  return *variance;
}

void calcul5(float seuil_confiance,
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
  boost::chrono::high_resolution_clock::time_point start_rng = boost::chrono::high_resolution_clock::now();
  float *N = (float *) calloc(NB_ACTIONS * nb_tirages * T, sizeof(float));
  float *TIRAGES = (float *) calloc(nb_tirages, sizeof(float));
  boost::mt19937 rng;
  boost::normal_distribution<> nd(0.0, 1.0);
  boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
  for(int g = 0; g < NB_ACTIONS * nb_tirages * T; g++) {
    N[g] = var_nor();
  }
  boost::chrono::nanoseconds ns_rng = boost::chrono::high_resolution_clock::now() - start_rng;
  // ~~~~~~~~~~~~~~~~~~~~~ OpenCL ~~~~~~~~~~~~~~~~~~~~~
  CLManager clm;
  std::string nom_kernel("calcul_trajectoires");
  clm.init(0,1,ENABLE_PROFILING);
  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/var-mc.cl");
  //  clm.loadKernels("kernels/var-mc.cl");

  //  clm.init(0,0,ENABLE_PROFILING);
  //  clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/var-mc.cl");
  //  clm.loadKernels("kernels/var-mc.cl");

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

  int  nombre_TIRAGES_par_Thread = 1;
  float esperance=0;
  float variance=0;
  int   nb_THREAD = nb_tirages;
  float *ESPERANCE = (float *) calloc(nb_tirages, sizeof(float));

  esperance = calculEsperance(TIRAGES,&nb_tirages,&nombre_TIRAGES_par_Thread,&nb_THREAD,ESPERANCE);
  variance =  calculVariance(TIRAGES,&nb_tirages,&nombre_TIRAGES_par_Thread,esperance,&nb_THREAD,ESPERANCE);
  
  // fin calcul de variance 
  /*   zone de  test ! 
  float test=0.0;
  float test2=0.0;
  for(int i =0; i < nb_tirages-3; i++){
    test+=TIRAGES[i]/(nb_tirages);
  }
  for(int i =0; i < nb_tirages-3; i++){
    test2+=(TIRAGES[i]-test)*(TIRAGES[i]-test)/(nb_tirages-1.0);
  }
 
  std::cout << "esp calculer cpu " << test << "var cpu" << test2 << std::endl;
  */
  // ~~~~~~~~~~~~~~ post-traitement VaR ~~~~~~~~~~~~~~~
  boost::chrono::high_resolution_clock::time_point start_sort = boost::chrono::high_resolution_clock::now();
  std::sort(TIRAGES, TIRAGES+nb_tirages);
  boost::chrono::nanoseconds ns_sort = boost::chrono::high_resolution_clock::now() - start_sort;
  
  int percentile = nb_tirages * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  float t_rng = ns_rng.count() / 1000000.0;
  float t_sort = ns_sort.count() / 1000000.0;
  std::cout << nb_tirages << ";";
  std::cout << P.getRendement() << ";";
  std::cout << TIRAGES[percentile+1] << ";";
  std::cout << NB_ACTIONS * nb_tirages * T * sizeof(float) << ";";
  std::cout << t_rng << ";";
  std::cout << clm.getGpuTime() << ";";
  std::cout << t_sort << std::endl;
}

void calcul2(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug) {
  boost::chrono::high_resolution_clock::time_point start_all = boost::chrono::high_resolution_clock::now();
  Portefeuille P(portefeuille);
  float *RENDEMENTS = P.getRendements();
  float *VOLS = P.getVolatilites();
  float *TI = P.getTauxInterets();
  int NB_ACTIONS = P.getTaille();
  float rendement = P.getRendement();
  float *TIRAGES = (float *) calloc(nb_tirages, sizeof(float));
  if (debug)
    {
      std::cout << "Mémoire utilisée:" << std::endl;
      int taille_rendement = sizeof(float);
      std::cout << "\trendement actuel: " << taille_rendement << " octets" << std::endl;
      int taille_rendements = sizeof(float) * NB_ACTIONS;
      std::cout << "\tRENDEMENTS: " << taille_rendements << " octets" << std::endl;
      int taille_vols = sizeof(float) * NB_ACTIONS;
      std::cout << "\tVOLS: " << taille_vols << " octets" << std::endl;
      int taille_ti = sizeof(float) * NB_ACTIONS;
      std::cout << "\tTI: " << taille_ti << " octets" << std::endl;
      int taille_nb_actions = sizeof(int);
      std::cout << "\tvariable nb_actions: " << taille_nb_actions << " octets" << std::endl;
      int taille_horizon = sizeof(int);
      std::cout << "\tvariable horizon: " << taille_horizon << " octets" << std::endl;
      int total = taille_rendements + taille_vols + taille_ti + taille_nb_actions + taille_horizon;
      std::cout << "\tTOTAL: " << total / 1000000.0 << " Mo" << std::endl;
    }
  // RNG + Tirages sur GPU
  CLManager clm;
  std::string nom_kernel("calcul_trajectoires2");
  clm.init(0,1,ENABLE_PROFILING);
  //clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/var-mc.cl");
  clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, NB_ACTIONS, sizeof(float), RENDEMENTS,false);
  clm.setKernelArg(nom_kernel, 1, NB_ACTIONS, sizeof(float), VOLS, false);
  clm.setKernelArg(nom_kernel, 2, NB_ACTIONS, sizeof(float), TI, false);
  clm.setKernelArg(nom_kernel, 3, nb_tirages, sizeof(float), TIRAGES, true); // sortie
  clm.setKernelArg(nom_kernel, 4, 1, sizeof(float), &rendement, false);
  clm.setKernelArg(nom_kernel, 5, 1, sizeof(int), &NB_ACTIONS, false);
  clm.setKernelArg(nom_kernel, 6, 1, sizeof(int), &T, false);
  clm.setKernelArg(nom_kernel, 7, 1, sizeof(int), &nb_tirages, false);
  clm.executeKernel(nb_tirages, nom_kernel);
  clm.getResultat();
  // tri sur CPU
  boost::chrono::high_resolution_clock::time_point start_sort = boost::chrono::high_resolution_clock::now();
  std::sort(TIRAGES, TIRAGES+nb_tirages);
  boost::chrono::nanoseconds ns_sort = boost::chrono::high_resolution_clock::now() - start_sort;

  //std::ofstream fd;
  //fd.open("tirages.data");
  FILE *fd;
  fd = fopen("tirages.data", "w");
  for(int g = 0; g < nb_tirages; g++) {
    fprintf(fd, "%f\n", TIRAGES[g]);
    //fd << TIRAGES[g] << std::endl;
  }
  fclose(fd);
  //fd.close();
  
  // VaR
  int percentile = nb_tirages * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  float t_sort = ns_sort.count() / 1000000.0;
  boost::chrono::nanoseconds ns_all = boost::chrono::high_resolution_clock::now() - start_all;
  float t_all = ns_all.count() / 1000000.0;
  std::cout << nb_tirages << ";";
  std::cout << P.getRendement() << ";";
  std::cout << TIRAGES[percentile+1] << ";";
  std::cout << clm.getGpuTime() << ";";
  std::cout << t_sort << ";";
  std::cout << t_all << std::endl;
}

void calcul1(float seuil_confiance,
	     int nb_tirages,
	     std::string portefeuille,
	     int T,
	     bool batch_mode) {
  boost::chrono::high_resolution_clock::time_point start_all = boost::chrono::high_resolution_clock::now();
  Portefeuille P(portefeuille);
  float rendement = P.getRendement();
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
  boost::chrono::high_resolution_clock::time_point start_rng = boost::chrono::high_resolution_clock::now();
  float *N = (float *) calloc(NB_ACTIONS * nb_tirages * T, sizeof(float));
  float *TIRAGES = (float *) calloc(nb_tirages, sizeof(float));
  boost::mt19937 rng;
  boost::normal_distribution<> nd(0.0, 1.0);
  boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
  for(int g = 0; g < NB_ACTIONS * nb_tirages * T; g++) {
    N[g] = var_nor();
  }
  boost::chrono::nanoseconds ns_rng = boost::chrono::high_resolution_clock::now() - start_rng;
  // ~~~~~~~~~~~~~~~~~~~~~ OpenCL ~~~~~~~~~~~~~~~~~~~~~
  CLManager clm;
  std::string nom_kernel("calcul_trajectoires");
  clm.init(0,1,ENABLE_PROFILING);
  //clm.loadKernels("/home/paittaha/var-monte-carlo-opencl/code/kernels/var-mc.cl");
  clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, NB_ACTIONS, sizeof(float), RENDEMENTS,false);
  clm.setKernelArg(nom_kernel, 1, NB_ACTIONS, sizeof(float), VOLS, false);
  clm.setKernelArg(nom_kernel, 2, NB_ACTIONS, sizeof(float), TI, false);
  clm.setKernelArg(nom_kernel, 3, NB_ACTIONS * nb_tirages * T, sizeof(float), N, false);
  clm.setKernelArg(nom_kernel, 4, 1, sizeof(float), &rendement, false);
  clm.setKernelArg(nom_kernel, 5, nb_tirages, sizeof(float), TIRAGES, true); // sortie
  clm.setKernelArg(nom_kernel, 6, 1, sizeof(int), &NB_ACTIONS, false);
  clm.setKernelArg(nom_kernel, 7, 1, sizeof(int), &T, false);
  // run sur le GPU
  clm.executeKernel(nb_tirages, nom_kernel);
  // recuperation des résultats
  clm.getResultat();
  
  // ~~~~~~~~~~~~~~ post-traitement VaR ~~~~~~~~~~~~~~~
  boost::chrono::high_resolution_clock::time_point start_sort = boost::chrono::high_resolution_clock::now();
  std::sort(TIRAGES, TIRAGES+nb_tirages);
  boost::chrono::nanoseconds ns_sort = boost::chrono::high_resolution_clock::now() - start_sort;
  std::ofstream fd;
  fd.open("tirages.data");
  for(int g = 0; g < nb_tirages; g++) {
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
  int percentile = nb_tirages * int(1.0 - seuil_confiance);
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  float t_rng = ns_rng.count() / 1000000.0;
  float t_sort = ns_sort.count() / 1000000.0;
  boost::chrono::nanoseconds ns_all = boost::chrono::high_resolution_clock::now() - start_all;
  float t_all = ns_all.count() / 1000000.0;
  std::cout << nb_tirages << ";";
  std::cout << P.getRendement() << ";";
  std::cout << TIRAGES[percentile+1] << ";";
  std::cout << NB_ACTIONS * nb_tirages * T * sizeof(float) << ";";
  std::cout << t_rng << ";";
  std::cout << clm.getGpuTime() << ";";
  std::cout << t_sort << ";";
  std::cout << t_all << std::endl;
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

// tirage de variables aléatoires gaussiennes
void distributionGaussienne(float seuil_conficance,int nb_tirages,std::string portefeuille,int T,bool debug) {
  CLManager clm;
  clm.init(0,1);
  std::string nom_kernel("distribution_gauss");
  clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  float *TIRAGES = NULL;
  int moitie = (nb_tirages / 2);
  TIRAGES = (float *) calloc(nb_tirages, sizeof(float));
  clm.setKernelArg(nom_kernel, 0, nb_tirages, sizeof(float), TIRAGES, true);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), &moitie, false);
  clm.executeKernel(moitie, nom_kernel);
  clm.getResultat();
  std::ofstream fd;
  fd.open("tirages.data");
  for(int g = 0; g < nb_tirages; g++) {
    //printf("%f ", TIRAGES[g]);
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
}

// estimation de Pi
void estimationPi(float seuil_confiance,int nb_tirages,std::string portefeuille,int T,bool debug) {
  CLManager clm;
  clm.init(0,1,ENABLE_PROFILING);
  int ul_nb_tirages = nb_tirages;
  int offset = 0;
  int *acc = NULL;
  acc = (int *) calloc(nb_tirages, sizeof(int));
  std::string nom_kernel("EstimatePi");
  clm.loadKernels("kernels/var-mc.cl");
  clm.compileKernel(nom_kernel);
  clm.setKernelArg(nom_kernel, 0, 1, sizeof(int), &ul_nb_tirages, false);
  clm.setKernelArg(nom_kernel, 1, 1, sizeof(int), &offset, false);
  clm.setKernelArg(nom_kernel, 2, nb_tirages, sizeof(int), acc, true);
  clm.executeKernel(nb_tirages, nom_kernel);
  clm.getResultat();
  float total = 0;
  for (int i = 0; i < nb_tirages; i++) {
    total += acc[i];
  }
  float frac = total / (float)ul_nb_tirages;
  printf("PI: %f\n", 4 * frac);
}







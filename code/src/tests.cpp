/*
 * tests.cpp
 *
 *  Created on: 5 mai 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include "tests.h"
#include <fstream>
#include <streambuf>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/chrono.hpp>
#include "CLManager.h"
#include "Portefeuille.h"

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
    fd << TIRAGES[g] << std::endl;
  }
  fd.close();
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
      std::cout << P << std::endl;
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

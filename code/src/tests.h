/*
 * tests.h
 *
 *  Created on: 5 mai. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#ifndef TESTS_H_
#define TESTS_H_
#include <string>


// RNG sur CPU, calcul trajectoires sur GPU, tri sur CPU
void calcul1(
		float seuil_confiance,
		int nb_tirages,
		std::string portefeuille,
		int T,
		bool debug);

// RNG sur GPU, calcul trajectoires sur GPU, tri sur CPU
void calcul2(
		float seuil_confiance,
		int nb_tirages,
		std::string portefeuille,
		int T,
		bool debug);

// calcul 1 + variance et interval de confiance
void calcul5(
		float seuil_confiance,
		int nb_tirages,
		std::string portefeuille,
		int T,
		bool debug);

// calcul de variance
float calculVariance(
		float *TIRAGES,
		int *nb_Simulation,
		int *nb_value_par_thread,
		float esperance,
		int *nb_THREAD,
		float *ESPERANCE);

// calcul de l'espérance
float calculEsperance(
		float *TIRAGES,
		int *nb_Simulation,
		int *nb_value_par_thread,
		int *nb_THREAD,
		float *ESPERANCE);

// estimation de PI par MC sur GPU
void estimationPi(
		float seuil_confiance,
		int nb_tirages,
		std::string portefeuille,
		int T,
		bool debug);

// distribution gaussienne
void distributionGaussienne(
		float seuil_confiance,
		int nb_tirages,
		std::string portefeuille,
		int T,
		bool debug);

#endif /* TESTS_H_ */

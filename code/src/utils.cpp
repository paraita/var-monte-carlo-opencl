/*
 * utils.cpp
 *
 *  Created on: 5 mai 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include "utils.h"
#include "CLManager.h"
#include <iostream>


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

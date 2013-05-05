/*
 * utils.h
 *
 *  Created on: 5 mai. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

// parsing des parametres d'entree
bool parse_args(
		int argc,char** argv,
		float* seuil_confiance,
		int* nb_tirages,
		std::string* p,
		int* horizon,
		bool* batch_mode);


#endif /* UTILS_H_ */

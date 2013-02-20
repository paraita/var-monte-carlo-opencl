/*
 * Actif.h
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#ifndef ACTIF_H_
#define ACTIF_H_
#include <string>
#include <iostream>

class Actif {
  friend std::ostream& operator<<(std::ostream&, Actif);
private:
	float volatilite;
	float rendement;
	float tauxInteret;
	std::string nom;
public:
	Actif();
	Actif(float v, float n, float t, std::string s);
	const std::string& getNom() const;
	void setNom(const std::string& nom);
	float getRendement() const;
	void setRendement(float rendement);
	float getTauxInteret() const;
	void setTauxInteret(float tauxInteret);
	float getVolatilite() const;
	void setVolatilite(float volatilite);
};

#endif /* ACTIF_H_ */

/*
 * Portefeuille.h
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#ifndef PORTEFEUILLE_H_
#define PORTEFEUILLE_H_
#include "Actif.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "stdlib.h"

class Portefeuille {
  friend std::ostream& operator<<(std::ostream&, Portefeuille);
 public:
  Portefeuille(std::vector<Actif> p);
  Portefeuille(std::string path);
  const std::vector<Actif>& getPortefeuille() const;
  void setPortefeuille(const std::vector<Actif>& portefeuille);
  unsigned int getTaille();
  float *getVolatilites();
  float *getRendements();
  float *getTauxInterets();
  float getRendement();
 private:
  std::vector<Actif> portefeuille;
  void read(std::vector<Actif>& tab, std::string path);
  void readCSV(std::vector< Actif >& tab,std::string path);
};

#endif /* PORTEFEUILLE_H_ */

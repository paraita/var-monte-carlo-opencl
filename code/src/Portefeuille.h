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
/**
 * Portefeuille is the portfolio model.
 * It is just a container for multiple assets.
 */
class Portefeuille {

  friend std::ostream& operator<<(std::ostream&, Portefeuille);

 public:
  /**
   * Constructor
   * @param path relative or absolute path to the csv file containing all assets to load.
   */
  Portefeuille(std::string path);

  /**
   * Getter for contained assets
   * @return vector of all assets contained
   */
  const std::vector<Actif>& getPortefeuille() const;

  /**
   * Setter for assets.
   * @param portefeuille the vector of all assets.
   */
  void setPortefeuille(const std::vector<Actif>& portefeuille);

  /**
   * Get the number of assets contained
   * @return the number of assets contained
   */
  unsigned int getTaille();

  /**
   * Getter for the assets volatilities.
   * @return an array of all assets volatilities.
   */
  float *getVolatilites();

  /**
   * Getter for the assets values.
   * @return an array of all assets values.
   */
  float *getRendements();

  /**
   * Getter for the assets risk free interest rates.
   * @return an array of all risk free interest rates.
   */
  float *getTauxInterets();

  /**
   * Calculate and returns the total portfolio value.
   * @return the value of the portfolio.
   */
  float getRendement();

 private:

  /**
   * Container of assets
   */
  std::vector<Actif> portefeuille;

  /**
   * Read file associated to the path variable and parse data to the tab variable.
   * @param tab the vector container that will hold all assets parsed in the file associated to the path variable.
   * @param path the full or relative path to a csv file containing assets.
   */
  void read(std::vector<Actif>& tab, std::string path);

  /**
   * Read file associated to the path variable and parse data to the tab variable.
   * @param tab the vector container that will hold all assets parsed in the file associated to the path variable.
   * @param path the full or relative path to a csv file containing assets.
   * @deprecated
   */
  void readCSV(std::vector< Actif >& tab,std::string path);
};

#endif /* PORTEFEUILLE_H_ */

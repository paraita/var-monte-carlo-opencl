/*
 * Portefeuille.cpp
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#include "Portefeuille.h"
#include "Actif.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <cstdlib>

Portefeuille::Portefeuille(std::vector<Actif> p){
  portefeuille=p;
}

Portefeuille::Portefeuille(std::string path){
	//readCSV(portefeuille,path);
	read(portefeuille,path);
}

const std::vector<Actif>& Portefeuille::getPortefeuille() const {
  return portefeuille;
}

void Portefeuille::setPortefeuille(const std::vector<Actif>& portefeuille) {
  this->portefeuille = portefeuille;
}

float* Portefeuille::getVolatilites(){
  float *tabVola= new float[portefeuille.size()];
  unsigned int i;
  for (i = 0; i < portefeuille.size(); ++i) {
	  Actif tmp = portefeuille[i];
	  tabVola[i] = tmp.getVolatilite();
  }
  return tabVola;
}

float* Portefeuille::getRendements(){
  float *tabRendement= new float[portefeuille.size()];
  unsigned int i;
  for (i = 0; i < portefeuille.size(); ++i) {
	  Actif tmp = portefeuille[i];
	  tabRendement[i] = tmp.getRendement();
  }
  return tabRendement;
}

float* Portefeuille::getTauxInterets(){
  float *tabtaux= new float[portefeuille.size()];
  unsigned int i;
  for (i = 0; i < portefeuille.size(); ++i) {
	  Actif tmp = portefeuille[i];
	  tabtaux[i] = tmp.getTauxInteret();
  }
  return tabtaux;
}

float Portefeuille::getRendement() {
  float rendement_portefeuille = 0;
  for (int i = 0; i < portefeuille.size(); i++) {
	  Actif tmp = portefeuille[i];
	  rendement_portefeuille += tmp.getRendement();
  }
  return rendement_portefeuille;
}

void Portefeuille::read(std::vector<Actif>& tab, std::string path) {
	std::ifstream ifs(path.c_str());
	if (!ifs) {
		std::cerr << "Impossible d'ouvrir " << path << std::endl;
		return;
	}
	std::string line;
	// on saute la premiere ligne
	std::getline(ifs,line);
	std::cout << "DEBUG:" << std::endl;
	std::cout << "premiere ligne: " << line << std::endl;
	while (std::getline(ifs, line)) {

		std::vector<std::string> actif;
		boost::split(actif, line, boost::is_any_of(";"), boost::token_compress_on);

		if (actif.size() < 5) {
			std::cerr << path << " mal formed" << std::endl;
			return;
		}
		float volatilite = atof(actif.at(2).c_str());
		float rendement = atof(actif.at(1).c_str());
		float ti = atof(actif.at(3).c_str());
		std::string nom = actif.at(0);
		int poids = atoi(actif.at(4).c_str());
		std::cout << "-----------------------------------" << std::endl;
		std::cout << "volatilite:" << volatilite << std::endl;
		std::cout << "rendement:" << rendement << std::endl;
		std::cout << "ti:" << ti << std::endl;
		std::cout << "nom:" << nom << std::endl;
		std::cout << "poids:" << poids << std::endl;
		std::cout << "-----------------------------------" << std::endl;
		Actif tmp(volatilite,
				  rendement,
				  ti,
				  nom,
				  poids);
		tab.push_back(tmp);
		line = "";
	}
	ifs.close();
}

void Portefeuille::readCSV(std::vector< Actif >& tab,std::string path) {
  std::ifstream ifs(path.c_str());
  if (!ifs)
    {
      std::cerr << "Impossible d'ouvrir le fichier " << path << std::endl;
      return;
    }
  std::string line;
  if(std::getline(ifs,line)){}
  while (std::getline(ifs,line)) // on itere sur les lignes
    {
      std::stringstream  lineStream(line);
      std::string cell, nom;
      float rendement, volatilite , ti;
      int iterateur = 0;
      while(getline(lineStream,cell, ';'))
	{
	  if(iterateur == 0){
	    nom=cell;
	  }else if(iterateur == 1){
	    rendement=::atof(cell.c_str());
	  }else if(iterateur == 2){
	    volatilite=::atof(cell.c_str());
	  }else if(iterateur == 3){
	    ti=::atof(cell.c_str());
	    Actif tmp(volatilite,rendement,ti,nom,1);
	    tab.push_back(tmp);
	    iterateur=0;
	  }
	  ++iterateur;
	}
    }
  ifs.close ();
}

unsigned int Portefeuille::getTaille() {
  return portefeuille.size();
}

std::ostream& operator<<(std::ostream& os, Portefeuille p) {
  std::vector<Actif> actifs = p.getPortefeuille();
  os << "Ptf:[" << std::endl;
  for (int i = 0; i < actifs.size(); i++) {
    os << actifs[i];
    os << std::endl;
  }
  os << "]";
  return os;
}

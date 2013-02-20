/*
 * Portefeuille.cpp
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#include "Portefeuille.h"

Portefeuille::Portefeuille(std::vector<Actif> p){
  portefeuille=p;
}

Portefeuille::Portefeuille(std::string path){
  readCSV(portefeuille,path);
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
    tabVola[i]=portefeuille[i].getVolatilite();
  }
  return tabVola;
}

float* Portefeuille::getRendements(){
  float *tabRendement= new float[portefeuille.size()];
  unsigned int i;
  for (i = 0; i < portefeuille.size(); ++i) {
    tabRendement[i]=portefeuille[i].getRendement();
  }
  return tabRendement;
}

float* Portefeuille::getTauxInterets(){
  float *tabtaux= new float[portefeuille.size()];
  unsigned int i;
  for (i = 0; i < portefeuille.size(); ++i) {
    tabtaux[i]=portefeuille[i].getTauxInteret();
  }
  return tabtaux;
}

float Portefeuille::getRendement() {
  float rendement_portefeuille = 0;
  for (int i = 0; i < portefeuille.size(); i++) {
    rendement_portefeuille += portefeuille[i].getRendement();
  }
  return rendement_portefeuille;
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
	    Actif tmp(volatilite,rendement,ti,nom);
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
  os << "Portefeuille:[" << std::endl;
  for (int i = 0; i < actifs.size(); i++) {
    os << actifs[i];
    os << std::endl;
  }
  os << "]";
  return os;
}

/*
 * Actif.cpp
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#include <string>
#include <iostream>
#include "Actif.h"

Actif::Actif(const Actif& a) {
	this->volatilite = a.getVolatilite();
	this->rendement = a.getRendement();
	this->tauxInteret = a.getTauxInteret();
	this->nom = a.getNom();
	this->poids = a.getPoids();
}

Actif::Actif(float v, float n, float t, std::string s, int p){
  volatilite=v;
  rendement=n;
  tauxInteret=t;
  poids = p;
  nom=s;
}

const std::string& Actif::getNom() const {
  return nom;
}

void Actif::setNom(const std::string& nom) {
  this->nom = nom;
}

float Actif::getRendement() const {
  return rendement;
}

void Actif::setRendement(float rendement) {
  this->rendement = rendement;
}

float Actif::getTauxInteret() const {
  return tauxInteret;
}

void Actif::setTauxInteret(float tauxInteret) {
  this->tauxInteret = tauxInteret;
}

float Actif::getVolatilite() const {
  return volatilite;
}

void Actif::setVolatilite(float volatilite) {
  this->volatilite = volatilite;
}

int Actif::getPoids() const {
	return poids;
}

void Actif::setPoids(int poids) {
	this->poids = poids;
}

std::ostream& operator<<(std::ostream& os, Actif a) {
  os << "{ Asset: " << a.getNom() << " [";
  os << "value:" << a.getRendement() ;
  os << ", volat:" << a.getVolatilite() ;
  os << ", rate:" << a.getTauxInteret();
  os << ", weight:" << a.getPoids() << "]}";
  return os;
}

/*
 * Actif.cpp
 *
 *  Created on: 19 févr. 2013
 *      Author: pierre
 */

#include <string>
#include <iostream>
#include "Actif.h"

Actif::Actif() {
  volatilite=0;
  rendement=0;
  tauxInteret=0;
  nom="";
}

Actif::Actif(float v, float n, float t, std::string s){
  volatilite=v;
  rendement=n;
  tauxInteret=t;
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

std::ostream& operator<<(std::ostream& os, Actif a) {
  os << "{ Actif:" << a.getNom() << " [";
  os << "rendement:" << a.getRendement() << ",taux:" << a.getTauxInteret() << "]}";
  return os;
}

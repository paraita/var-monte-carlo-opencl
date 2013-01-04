/*
 * main.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include <iostream>
#include <string>
#include <CL/cl.h>
#include "CLManager.h"


int main(int argc, char *argv[])
{
  CLManager clm;
  clm.printPlatform();
  return 0;
}



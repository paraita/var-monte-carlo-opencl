/*
 * CLManager.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include <iostream>
#include <string>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"

void CLManager::err_check(const cl_int status, const std::string msg, const bool exitOnErr) {
  if (status != CL_SUCCESS) {
    std::cout << "Erreur OpenCL: " << status << " (" << msg << ")" << std::endl;
    if (exitOnErr) {
      exit(-1);
    }
  }
}

void CLManager::printPlatform()
{
  cl_int err = 0;
  cl_uint numPlatforms = 0;
  cl_uint numDevices = 0;
  size_t paramSize = 0;
  cl_platform_id *platforms = NULL;
  cl_device_id *devices = NULL;
  char *platformName = NULL;
  char *platformVendor = NULL;
  char *deviceName = NULL;
  char *deviceVendor = NULL;
  char *deviceVersion = NULL;
  char *deviceDriverVersion = NULL;
  cl_uint deviceMaxCU = 0;
  size_t deviceMaxWG = 0;

  // je recup le nombre de plateformes dispo
  err = clGetPlatformIDs(0, NULL, &numPlatforms);
  err_check(err, "recup du nombre de plateformes dispo", true);
  if (numPlatforms <= 0) {
    std::cout << "aucune plateforme OpenCL dispo !" << std::endl;
  }

  platforms = (cl_platform_id*) calloc(numPlatforms, sizeof(cl_platform_id));

  // je recup toutes les plateformes
  err = clGetPlatformIDs(numPlatforms, platforms, NULL);
  err_check(err, "recup de toutes les plateformes", true);

  for (unsigned int i = 0; i < numPlatforms; i++) {

    // recup nom de plateforme
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &paramSize);
    err_check(err, "recup de la taille du nom de plateforme", true);
    platformName = (char *)malloc(paramSize);
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, paramSize, platformName, NULL);
    err_check(err, "recup du nom de plateforme", true);

    // recup vendor plateforme
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &paramSize);
    err_check(err, "recup de la taille du vendor de plateforme", true);
    platformVendor = (char *)malloc(paramSize);
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, paramSize, platformVendor, NULL);
    err_check(err, "recup du vendor de plateforme", true);

    std::cout << "Plateforme: " << platformName << " (" << platformVendor << ")" << std::endl;

    // recup nb devices
    err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    err_check(err, "recup de la taille du nombre de devices", true);
    devices = (cl_device_id*) calloc(numDevices, sizeof(cl_device_id));
    err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    err_check(err, "recup du nombre de devices", true);

    for (unsigned int j = 0; j < numDevices; j++) {
      // recup nom device
      err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du nom du device", true);
      deviceName = (char *)malloc(paramSize);
      err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, paramSize, deviceName, NULL);
      err_check(err, "recup du nom du device", true);

      // recup du vendor device
      err = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du vendor du device", true);
      deviceVendor = (char *)malloc(paramSize);
      err = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, paramSize, deviceVendor, NULL);
      err_check(err, "recup du vendor du device", true);

      // recup de la version d'OpenCL supportee par le device
      err = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &paramSize);
      err_check(err, "recup de la taille de la version OpenCL du device", true);
      deviceVersion = (char *)malloc(paramSize);
      err = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, paramSize, deviceVersion, NULL);
      err_check(err, "recup de la version OpenCL du device", true);

      // recup de la version du driver du device
      err = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du driver du device", true);
      deviceDriverVersion = (char *)malloc(paramSize);
      err = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, paramSize, deviceDriverVersion, NULL);
      err_check(err, "recup de la version du driver du device", true);

      // recup de la taille du Compute Units
      err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &deviceMaxCU, NULL);
      err_check(err, "recup du CU du device", true);

      // recup de la taille du WG
      err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &deviceMaxWG, NULL);
      err_check(err, "recup du WG du device", true);

      std::cout << "\tDevice: " << deviceName << " - " << deviceVendor << std::endl;
      std::cout << "\t\tVersion OpenCL supportee: " << deviceVersion << std::endl;
      std::cout << "\t\tVersion Driver: " << deviceDriverVersion << std::endl;
      std::cout << "\t\tTaille Compute Units:" << deviceMaxCU << std::endl;
      std::cout << "\t\tTaille Work Group:" << deviceMaxWG << std::endl;

      free(deviceName);
      free(deviceVendor);
      free(deviceDriverVersion);
      free(deviceVersion);
    }
    free(platformName);
    free(platformVendor);
    free(devices);
  }
  free(platforms);
}





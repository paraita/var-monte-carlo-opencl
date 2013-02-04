/*
 * CLManager.cpp
 *
 *  Created on: 4 janv. 2013
 *      Author: "Paraita Wohler <paraita.wohler@gmail.com>"
 */

#include <iostream>
#include <fstream>
#include <string>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CLManager.h"

// constructeur par defaut
CLManager::CLManager() : platforms(),
			 devices() {
  cl_int err = 0;
  cl_uint numPlatforms = 0;
  cl_uint numDevices = 0;
  cl_platform_id *p = NULL;
  cl_device_id *d = NULL;
  cl_uint deviceMaxCU = 0;
  size_t deviceMaxWG = 0;
  
  // je recup le nombre de plateformes dispo
  err = clGetPlatformIDs(0, NULL, &numPlatforms);
  err_check(err, "recup du nombre de plateformes dispo", true);
  if (numPlatforms <= 0) {
    std::cout << "aucune plateforme OpenCL dispo !" << std::endl;
    exit(-1);
  }

  // je recup toutes les plateformes
  p = (cl_platform_id*) calloc(numPlatforms, sizeof(cl_platform_id));
  err = clGetPlatformIDs(numPlatforms, p, NULL);
  err_check(err, "recup de toutes les plateformes", true);

  // je garde toutes les plateformes dans un vector
  for (int i = 0; i < numPlatforms; i++) {
    platforms.push_back(p[i]);
  }

  // je recup tous les devices des plateformes
  size_t paramSize = 0;
  for (int i = 0; i < platforms.size(); i++) {
    
    // je recup le nombre de devices sur cette plateforme
    err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    err_check(err, "recup du nombre de devices", true);

    // je recup tous les devices de la plateforme en cours
    d = (cl_device_id*) calloc(numDevices, sizeof(cl_device_id));
    err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, d, NULL);
    err_check(err, "recup des devices", true);

    // je garde les devices dans un vector
    std::vector<cl_device_id> d_device;
    for (int j = 0; j < numDevices; j++) {
      d_device.push_back(d[j]);
      titoi = d[j];
    }
    // le vector des plateformes est indiced pareil que celui des devices
    devices.push_back(d_device);
  }
  
  free(p);
  free(d);
}

// cree le contexte et la command queue
void CLManager::init(const int platform, const int device) {
  cl_int err = 0;
  cl_device_id d = (devices[platform])[device];
  
  context = clCreateContext( NULL, 1, &d, NULL, NULL, &err);
  err_check(err, "creation du contexte opencl", true);
  
  command_queue = clCreateCommandQueue(context, d, 0, &err);
  err_check(err, "creation de la command queue", true);
}

void CLManager::check(const cl_int status,
		      const std::string msg) {
  err_check(status, msg, true);
}

void CLManager::err_check(const cl_int status,
			  const std::string msg,
			  const bool exitOnErr) {
  if (status != CL_SUCCESS) {
    std::cout << "Erreur OpenCL: " << status << " (" << msg << ")" << std::endl;
    if (exitOnErr) {
      exit(-1);
    }
  }
}

std::string CLManager::printPlatform() {
  std::stringstream res;
  cl_int err = 0;
  size_t paramSize = 0;
  char *platformName = NULL;
  char *platformVendor = NULL;
  char *platformVersion = NULL;
  char *deviceName = NULL;
  char *deviceVendor = NULL;
  char *deviceVersion = NULL;
  char *deviceDriverVersion = NULL;
  cl_uint deviceMaxCU = 0;
  size_t deviceMaxWG = 0;

  for (unsigned int i = 0; i < platforms.size(); i++) {

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
    
    // recup version plateforme
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 0, NULL, &paramSize);
    err_check(err, "recup de la taille de la version de plateforme", true);
    platformVersion = (char *)malloc(paramSize);
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, paramSize, platformVersion, NULL);
    err_check(err, "recup de la version de plateforme", true);

    res << "Plateforme[" << i << "]: ";
    res << platformName << " (" << platformVendor << ")" << std::endl;
    res << "Version d'OpenCL supportee: " << platformVersion << std::endl;

    std::vector<cl_device_id> d = devices[i];

    for (unsigned int j = 0; j < d.size(); j++) {
      // recup nom device
      err = clGetDeviceInfo(d[j], CL_DEVICE_NAME, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du nom du device", true);
      deviceName = (char *)malloc(paramSize);
      err = clGetDeviceInfo(d[j], CL_DEVICE_NAME, paramSize, deviceName, NULL);
      err_check(err, "recup du nom du device", true);

      // recup du vendor device
      err = clGetDeviceInfo(d[j], CL_DEVICE_VENDOR, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du vendor du device", true);
      deviceVendor = (char *)malloc(paramSize);
      err = clGetDeviceInfo(d[j], CL_DEVICE_VENDOR, paramSize, deviceVendor, NULL);
      err_check(err, "recup du vendor du device", true);

      // recup de la version d'OpenCL supportee par le device
      err = clGetDeviceInfo(d[j], CL_DEVICE_VERSION, 0, NULL, &paramSize);
      err_check(err, "recup de la taille de la version OpenCL du device", true);
      deviceVersion = (char *)malloc(paramSize);
      err = clGetDeviceInfo(d[j], CL_DEVICE_VERSION, paramSize, deviceVersion, NULL);
      err_check(err, "recup de la version OpenCL du device", true);

      // recup de la version du driver du device
      err = clGetDeviceInfo(d[j], CL_DRIVER_VERSION, 0, NULL, &paramSize);
      err_check(err, "recup de la taille du driver du device", true);
      deviceDriverVersion = (char *)malloc(paramSize);
      err = clGetDeviceInfo(d[j], CL_DRIVER_VERSION, paramSize, deviceDriverVersion, NULL);
      err_check(err, "recup de la version du driver du device", true);

      // recup de la taille du Compute Units
      err = clGetDeviceInfo(d[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &deviceMaxCU, NULL);
      err_check(err, "recup du CU du device", true);

      // recup de la taille du WG
      err = clGetDeviceInfo(d[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &deviceMaxWG, NULL);
      err_check(err, "recup du WG du device", true);

      res << "\t[" << j << "]Device: " << deviceName << " - " << deviceVendor << std::endl;
      res << "\t\tVersion OpenCL supportee: " << deviceVersion << std::endl;
      res << "\t\tVersion Driver: " << deviceDriverVersion << std::endl;
      res << "\t\tTaille Compute Units:" << deviceMaxCU << std::endl;
      res << "\t\tTaille Max Work Group:" << deviceMaxWG << std::endl;

      free(deviceName);
      free(deviceVendor);
      free(deviceDriverVersion);
      free(deviceVersion);
    }
    free(platformName);
    free(platformVendor);
    free(platformVersion);
  }
  return res.str();
}





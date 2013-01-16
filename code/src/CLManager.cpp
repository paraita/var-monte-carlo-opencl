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


// cree le contexte et la command queue
void CLManager::init(const cl_device_id device, cl_context *context, cl_command_queue *command_queue) {
  cl_int err = 0;
  //FIX: on prend toujours la premiere plateforme dispo
  *context = clCreateContext( NULL, 1, &device, NULL, NULL, &err);
  err_check(err, "creation du contexte opencl", true);
  
  *command_queue = clCreateCommandQueue(*context, device, 0, &err);
  err_check(err, "creation de la command queue", true);
}

void CLManager::getAllPlatforms(cl_uint *numPlatforms, cl_platform_id ** platforms) {
  cl_int err = 0;

  // je recup le nombre de plateformes dispo
  err = clGetPlatformIDs(0, NULL, numPlatforms);
  err_check(err, "recup du nombre de plateformes dispo", true);
  if (numPlatforms <= 0) {
    std::cout << "aucune plateforme OpenCL dispo !" << std::endl;
    exit(-1);
  }

  (*platforms) = (cl_platform_id*) calloc((*numPlatforms), sizeof(cl_platform_id));

  // je recup toutes les plateformes
  err = clGetPlatformIDs((*numPlatforms), (*platforms), NULL);
  err_check(err, "recup de toutes les plateformes", true);
}

void CLManager::getAllDevicesByPlatform(cl_platform_id platform, cl_uint *numDevices, cl_device_id ** devices) {
  cl_int err = 0;
  
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, numDevices);
  err_check(err, "recup du nombre de devices", true);

  (*devices) = (cl_device_id*) calloc(*numDevices, sizeof(cl_device_id));

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, *numDevices, *devices, NULL);
  err_check(err, "recup des devices", true);
}

void CLManager::check(const cl_int status, const std::string msg) {
  err_check(status, msg, true);
}

void CLManager::err_check(const cl_int status, const std::string msg, const bool exitOnErr) {
  if (status != CL_SUCCESS) {
    std::cout << "Erreur OpenCL: " << status << " (" << msg << ")" << std::endl;
    if (exitOnErr) {
      exit(-1);
    }
  }
}

void CLManager::printPlatform() {
  cl_int err = 0;
  cl_uint numPlatforms = 0;
  cl_uint numDevices = 0;
  size_t paramSize = 0;
  cl_platform_id *platforms = NULL;
  cl_device_id *devices = NULL;
  char *platformName = NULL;
  char *platformVendor = NULL;
  char *platformVersion = NULL;
  char *deviceName = NULL;
  char *deviceVendor = NULL;
  char *deviceVersion = NULL;
  char *deviceDriverVersion = NULL;
  cl_uint deviceMaxCU = 0;
  size_t deviceMaxWG = 0;
  
  // recup les plateformes
  getAllPlatforms(&numPlatforms, &platforms);

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
    
    // recup version plateforme
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 0, NULL, &paramSize);
    err_check(err, "recup de la taille de la version de plateforme", true);
    platformVersion = (char *)malloc(paramSize);
    err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, paramSize, platformVersion, NULL);
    err_check(err, "recup de la version de plateforme", true);

    std::cout << "Plateforme: " << platformName << " (" << platformVendor << ")" << std::endl;
    std::cout << "Version d'OpenCL supportee: " << platformVersion << std::endl;

    // recup devices
    getAllDevicesByPlatform(platforms[i], &numDevices, &devices);

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

      std::cout << "\t[" << j << "]Device: " << deviceName << " - " << deviceVendor << std::endl;
      std::cout << "\t\tVersion OpenCL supportee: " << deviceVersion << std::endl;
      std::cout << "\t\tVersion Driver: " << deviceDriverVersion << std::endl;
      std::cout << "\t\tTaille Compute Units:" << deviceMaxCU << std::endl;
      std::cout << "\t\tTaille Max Work Group:" << deviceMaxWG << std::endl;

      free(deviceName);
      free(deviceVendor);
      free(deviceDriverVersion);
      free(deviceVersion);
    }
    free(platformName);
    free(platformVendor);
    free(platformVersion);
    free(devices);
  }
  free(platforms);
}





/***********************************************************************************************************************************
CENTRO DE INVESTIGACION EN MATEMATICAS
DOCTORADO EN CIENCIAS DE LA COMPUTACION
FERNANDO CERVANTES SANCHEZ

FILE NAME : computeAUCROC.h

PURPOSE : Declares the python extensions to compute the area under the ROC curve.

FILE REFERENCES :
Name        I / O        Description
None----       ----------

ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES :
None
************************************************************************************************************************************/
#ifndef COMPUTE_AUCROC_H_INCLUDED
#define COMPUTE_AUCROC_H_INCLUDED

#ifdef BUILDING_PYTHON_MODULE
	#include <Python.h>
	#include <numpy/ndarraytypes.h>
	#include <numpy/ufuncobject.h>
	#include <numpy/npy_3kcompat.h>
	#define COMPUTEAUCROC_DLL 
#else
	#if defined(_WIN32) || defined(_WIN64)
		#ifdef BUILDING_COMPUTEAUCROC_DLL
			#define COMPUTEAUCROC_DLL __declspec(dllexport)
		#else
			#define COMPUTEAUCROC_DLL __declspec(dllimport)
		#endif
	#else
		#define COMPUTEAUCROC_DLL 
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>

typedef struct roc_pair
{
	double classification_response;
	char ground_truth_class;
} roc_pair;


int compROCpairs(const void * pair_a_ptr, const void * pair_b_ptr);

double COMPUTEAUCROC_DLL aucROC_impl(double * img_src, char * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs);
double COMPUTEAUCROC_DLL aucROCsavefile_impl(double * img_src, char * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename);
double COMPUTEAUCROC_DLL aucROCmasked_impl(double * img_src, char * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs);
double COMPUTEAUCROC_DLL aucROCmaskedsavefile_impl(double * img_src, char * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename);

#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROC(PyObject *self, PyObject *args);
static PyObject* aucROCsavefile(PyObject *self, PyObject *args);
static PyObject* aucROCmasked(PyObject *self, PyObject *args);
static PyObject* aucROCmaskedsavefile(PyObject *self, PyObject *args);
#endif

#endif // COMPUTE_AUCROC_H_INCLUDED

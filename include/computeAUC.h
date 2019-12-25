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
    #define COMPUTEAUCROC_DLL_PUBLIC
    #define COMPUTEAUCROC_DLL_LOCAL 
#else
	#if defined(_WIN32) || defined(_WIN64)
		#ifdef BUILDING_COMPUTEAUCROC_DLL
            #ifdef __GNUC__
                #define COMPUTEAUCROC_DLL_PUBLIC __attribute__ ((dllexport))
            #else
                #define COMPUTEAUCROC_DLL_PUBLIC __declspec(dllexport)
            #endif
        #else
            #ifdef __GNUC__
                #define COMPUTEAUCROC_DLL_PUBLIC __attribute__ ((dllimport))
            #else
                #define COMPUTEAUCROC_DLL_PUBLIC __declspec(dllimport)
            #endif
        #endif
        #define COMPUTEAUCROC_DLL_LOCAL
	#else
		#if __GNUC__ >= 4
            #define COMPUTEAUCROC_DLL_PUBLIC __attribute__ ((visibility ("default")))
            #define COMPUTEAUCROC_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
        #else
            #define COMPUTEAUCROC_DLL_PUBLIC
            #define COMPUTEAUCROC_DLL_LOCAL
        #endif
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
#define DEBMSG(MESSAGE) printf(MESSAGE)
#define DEBNUMMSG(MESSAGE, NUM) printf(MESSAGE, NUM);
#else
#define DEBMSG(MESSAGE) 
#define DEBNUMMSG(MESSAGE, NUM) 
#endif

typedef struct roc_pair
{
	double classification_response;
	double ground_truth_class;
} roc_pair;


int compROCpairs(const void * pair_a_ptr, const void * pair_b_ptr);

double COMPUTEAUCROC_DLL_PUBLIC aucROC_impl(double * img_src, double * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs);
double COMPUTEAUCROC_DLL_PUBLIC aucROCsavefile_impl(double * img_src, double * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename);
double COMPUTEAUCROC_DLL_PUBLIC aucROCmasked_impl(double * img_src, double * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs);
double COMPUTEAUCROC_DLL_PUBLIC aucROCmaskedsavefile_impl(double * img_src, double * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename);

#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROC(PyObject *self, PyObject *args);
static PyObject* aucROCsavefile(PyObject *self, PyObject *args);
static PyObject* aucROCmasked(PyObject *self, PyObject *args);
static PyObject* aucROCmaskedsavefile(PyObject *self, PyObject *args);
#endif

#endif // COMPUTE_AUCROC_H_INCLUDED

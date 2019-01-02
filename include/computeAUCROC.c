/***********************************************************************************************************************************
CENTRO DE INVESTIGACION EN MATEMATICAS
DOCTORADO EN CIENCIAS DE LA COMPUTACION
FERNANDO CERVANTES SANCHEZ

FILE NAME : computeAUCROC.cpp

PURPOSE : Defines the python extensions to compute the area under the ROC curve.

FILE REFERENCES :
Name        I / O        Description
None----       ----------

ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES :
None
************************************************************************************************************************************/
#include "computeAUCROC.h"


int compROCpairs(const void * pair_a_ptr, const void * pair_b_ptr)
{
	if (((roc_pair*)pair_a_ptr)->classification_response < ((roc_pair*)pair_b_ptr)->classification_response)
	{
		return -1;
	}
	else if (((roc_pair*)pair_a_ptr)->classification_response == ((roc_pair*)pair_b_ptr)->classification_response)
	{
		return 0;
	}

	return 1;
}




double aucROC_impl(double * img_src, double * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs)
{
	roc_pair * responses_pairs = (roc_pair*)malloc(n_imgs * height * width * sizeof(roc_pair));
	double * img_src_ptr = img_src;
	double * groundtruth_ptr = groundtruth;

	double total_positive = 0.0;
	for (unsigned int i = 0; i < n_imgs * height * width; i++, img_src_ptr++, groundtruth_ptr++)
	{
		(responses_pairs + i)->ground_truth_class = *groundtruth_ptr;
		(responses_pairs + i)->classification_response = *img_src_ptr;

		if (*groundtruth_ptr > 0)
		{
			total_positive += 1.0;
		}
	}

	double total_negative = (double)(n_imgs * height * width) - total_positive;

	qsort(responses_pairs, n_imgs * height * width, sizeof(roc_pair), compROCpairs);

	double area_under_curve = 0.0;

	double accumulated_false_negative = responses_pairs->ground_truth_class > 0.0 ? 1.0 : 0.0;
	double accumulated_true_negative = responses_pairs->ground_truth_class < 0.0 ? 1.0 : 0.0;

	double previous_true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
	double previous_false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

	double true_positive_fraction, false_positive_fraction;
	for (unsigned int i = 1; i < n_imgs * height * width; i++)
	{
		if ((responses_pairs + i)->ground_truth_class > 0)
		{
			accumulated_false_negative += 1.0;
		}
		else
		{
			accumulated_true_negative += 1.0;
		}

		true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
		false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

		area_under_curve -= (true_positive_fraction + previous_true_positive_fraction) * (false_positive_fraction - previous_false_positive_fraction);

		previous_true_positive_fraction = true_positive_fraction;
		previous_false_positive_fraction = false_positive_fraction;
	}

	free(responses_pairs);

	return area_under_curve * 0.5;
}



#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROC(PyObject *self, PyObject *args)
{

	PyArrayObject  *input_img;
	PyArrayObject  *groundtruth;
	
	npy_intp height, width, n_imgs = 1, n_channels = 1;
	
	/* Parse the input arguments to extract two numpy arrays: */
	if (!PyArg_ParseTuple(args, "O!O!", &PyArray_Type, &input_img, &PyArray_Type, &groundtruth))
	{
		return NULL;
	}
	
	if (input_img->nd > 3)
	{
		n_imgs = input_img->dimensions[0];
		n_channels = input_img->dimensions[1];
		height = input_img->dimensions[2];
		width = input_img->dimensions[3];		
	}
	else if (input_img->nd > 2)
	{
		n_imgs = input_img->dimensions[0];
		height = input_img->dimensions[1];
		width = input_img->dimensions[2];		
	}
	else
	{
		height = input_img->dimensions[0];
		width = input_img->dimensions[1];
	}
	
	char * input_img_data = input_img->data;
	char * groundtruth_data = groundtruth->data;
		
	npy_intp input_img_stride = input_img->strides[input_img->nd-1];
	npy_intp groundtruth_stride = groundtruth->strides[groundtruth->nd-1];
	
	double auc_resp = aucROC_impl((double*)input_img_data, (double*)groundtruth_data, height, width, n_imgs);

	return Py_BuildValue("d", auc_resp);
}
#endif



double aucROCsavefile_impl(double * img_src, double * groundtruth, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename)
{
	roc_pair * responses_pairs = (roc_pair*)malloc(n_imgs * height * width * sizeof(roc_pair));
	double * img_src_ptr = img_src;
	double * groundtruth_ptr = groundtruth;

	double total_positive = 0.0;
	for (unsigned int i = 0; i < n_imgs * height * width; i++, img_src_ptr++, groundtruth_ptr++)
	{
		(responses_pairs + i)->ground_truth_class = *groundtruth_ptr;
		(responses_pairs + i)->classification_response = *img_src_ptr;

		if (*groundtruth_ptr > 0)
		{
			total_positive += 1.0;
		}
	}

	double total_negative = (double)(n_imgs * height * width) - total_positive;

	qsort(responses_pairs, n_imgs * height * width, sizeof(roc_pair), compROCpairs);
	
	double area_under_curve = 0.0;

	double accumulated_false_negative = responses_pairs->ground_truth_class > 0.0 ? 1.0 : 0.0;
	double accumulated_true_negative = responses_pairs->ground_truth_class < 0.0 ? 1.0 : 0.0;

	double previous_true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
	double previous_false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

	FILE * fp_roc = fopen(filename, "w");
	fprintf(fp_roc, "%.12f %.12f %.12f\n", previous_false_positive_fraction, previous_true_positive_fraction, area_under_curve);

	double true_positive_fraction, false_positive_fraction;
	for (unsigned int i = 1; i < n_imgs * height * width; i++)
	{
		if ((responses_pairs + i)->ground_truth_class > 0)
		{
			accumulated_false_negative += 1.0;
		}
		else
		{
			accumulated_true_negative += 1.0;
		}

		true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
		false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;
		
		area_under_curve -= (true_positive_fraction + previous_true_positive_fraction) * (false_positive_fraction - previous_false_positive_fraction);

		fprintf(fp_roc, "%.12f %.12f %.12f\n", false_positive_fraction, true_positive_fraction, area_under_curve);

		previous_true_positive_fraction = true_positive_fraction;
		previous_false_positive_fraction = false_positive_fraction;
	}

	fclose(fp_roc);

	free(responses_pairs);

	return area_under_curve * 0.5;
}



#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROCsavefile(PyObject *self, PyObject *args)
{

	PyArrayObject  *input_img;
	PyArrayObject  *groundtruth;
	const char * roc_curve_filename;
	
	npy_intp height, width, n_imgs = 1, n_channels = 1;
	
	/* Parse the input arguments to extract two numpy arrays: */
	if (!PyArg_ParseTuple(args, "O!O!s", &PyArray_Type, &input_img, &PyArray_Type, &groundtruth, &roc_curve_filename))
	{
		return NULL;
	}

	if (input_img->nd > 3)
	{
		n_imgs = input_img->dimensions[0];
		n_channels = input_img->dimensions[1];
		height = input_img->dimensions[2];
		width = input_img->dimensions[3];		
	}
	else if (input_img->nd > 2)
	{
		n_imgs = input_img->dimensions[0];
		height = input_img->dimensions[1];
		width = input_img->dimensions[2];		
	}
	else
	{
		height = input_img->dimensions[0];
		width = input_img->dimensions[1];
	}
	
	char * input_img_data = input_img->data;
	char * groundtruth_data = groundtruth->data;
		
	npy_intp input_img_stride = input_img->strides[input_img->nd-1];
	npy_intp groundtruth_stride = groundtruth->strides[groundtruth->nd-1];
	
	double auc_resp = aucROCsavefile_impl((double*)input_img_data, (double*)groundtruth_data, height, width, n_imgs, roc_curve_filename);

	return Py_BuildValue("d", auc_resp);
}
#endif



double aucROCmasked_impl(double * img_src, double * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs)
{
	roc_pair * responses_pairs = (roc_pair*)malloc(n_imgs * height * width * sizeof(roc_pair));
	double * img_src_ptr = img_src;
	double * groundtruth_ptr = groundtruth;
	char * mask_ptr = mask;

DEBNUMMSG("n_imgs: %i, ", n_imgs);
DEBNUMMSG("%ix", height);
DEBNUMMSG("%i\n", width);

	unsigned int active_portion = 0;
	double total_positive = 0.0;
	for (unsigned int i = 0; i < n_imgs * height * width; i++, img_src_ptr++, groundtruth_ptr++, mask_ptr++)
	{
		if ((int)*mask_ptr > 0)
		{
			(responses_pairs + active_portion)->classification_response = *img_src_ptr;
			(responses_pairs + active_portion)->ground_truth_class = *groundtruth_ptr;

			if (*groundtruth_ptr > 0)
			{
				total_positive += 1.0;
			}
			
			active_portion++;
		}
	}

	double total_negative = (double)active_portion - total_positive;

DEBNUMMSG("active_portion: %i\n", active_portion);
DEBNUMMSG("total_positive: %f\n", total_positive);
DEBNUMMSG("total_negative: %f\n", total_negative);

	qsort(responses_pairs, active_portion, sizeof(roc_pair), compROCpairs);
	
	double area_under_curve = 0.0;

	double accumulated_false_negative = responses_pairs->ground_truth_class > 0.0 ? 1.0 : 0.0;
	double accumulated_true_negative = responses_pairs->ground_truth_class < 0.0 ? 1.0 : 0.0;

	double previous_true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
	double previous_false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

	double true_positive_fraction, false_positive_fraction;
	for (unsigned int i = 1; i < active_portion; i++)
	{
		if ((responses_pairs + i)->ground_truth_class > 0)
		{
			accumulated_false_negative += 1.0;
		}
		else
		{
			accumulated_true_negative += 1.0;
		}

		true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
		false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;
		
		area_under_curve -= (true_positive_fraction + previous_true_positive_fraction) * (false_positive_fraction - previous_false_positive_fraction);

		previous_true_positive_fraction = true_positive_fraction;
		previous_false_positive_fraction = false_positive_fraction;
	}
	
	free(responses_pairs);

DEBNUMMSG("Area under the ROC curve: %f\n", area_under_curve);

	return area_under_curve * 0.5;
}



#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROCmasked(PyObject *self, PyObject *args)
{

	PyArrayObject *input_img;
	PyArrayObject *groundtruth;
	PyArrayObject *mask;

	npy_intp height, width, n_imgs = 1, n_channels = 1;
	
	/* Parse the input arguments to extract two numpy arrays: */
	if (!PyArg_ParseTuple(args, "O!O!O!", &PyArray_Type, &input_img, &PyArray_Type, &groundtruth, &PyArray_Type, &mask))
	{
		return NULL;
	}
	
	if (input_img->nd > 3)
	{
		n_imgs = input_img->dimensions[0];
		n_channels = input_img->dimensions[1];
		height = input_img->dimensions[2];
		width = input_img->dimensions[3];
	}
	else if (input_img->nd > 2)
	{
		n_imgs = input_img->dimensions[0];
		height = input_img->dimensions[1];
		width = input_img->dimensions[2];		
	}
	else
	{
		height = input_img->dimensions[0];
		width = input_img->dimensions[1];
	}
	
	char * input_img_data = input_img->data;
	char * groundtruth_data = groundtruth->data;
	char * mask_data = mask->data;
		
	npy_intp input_img_stride = input_img->strides[input_img->nd-1];
	npy_intp groundtruth_stride = groundtruth->strides[groundtruth->nd-1];
	npy_intp mask_stride = mask->strides[mask->nd-1];
	
	double auc_resp = aucROCmasked_impl((double*)input_img_data, (double*)groundtruth_data, mask_data, height, width, n_imgs);

	return Py_BuildValue("d", auc_resp);
}
#endif



double aucROCmaskedsavefile_impl(double * img_src, double * groundtruth, char * mask, const unsigned int height, const unsigned int width, const unsigned int n_imgs, const char * filename)
{
	roc_pair * responses_pairs = (roc_pair*)malloc(n_imgs * height * width * sizeof(roc_pair));
	double * img_src_ptr = img_src;
	double * groundtruth_ptr = groundtruth;
	char * mask_ptr = mask;

	unsigned int active_portion = 0;
	double total_positive = 0.0;
	for (unsigned int i = 0; i < n_imgs * height * width; i++, img_src_ptr++, groundtruth_ptr++, mask_ptr++)
	{
		if (*mask_ptr > 0)
		{
			(responses_pairs + active_portion)->ground_truth_class = *groundtruth_ptr;
			(responses_pairs + active_portion)->classification_response = *img_src_ptr;

			if (*groundtruth_ptr > 0)
			{
				total_positive += 1.0;
			}
			active_portion++;
		}
	}

	double total_negative = (double)active_portion - total_positive;

	qsort(responses_pairs, active_portion, sizeof(roc_pair), compROCpairs);

	double area_under_curve = 0.0;

	double accumulated_false_negative = responses_pairs->ground_truth_class > 0.0 ? 1.0 : 0.0;
	double accumulated_true_negative = responses_pairs->ground_truth_class < 0.0 ? 1.0 : 0.0;

	double previous_true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
	double previous_false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

	FILE * fp_roc = fopen(filename, "w");
	fprintf(fp_roc, "%.12f %.12f %.12f\n", previous_false_positive_fraction, previous_true_positive_fraction, area_under_curve);

	double true_positive_fraction, false_positive_fraction;
	for (unsigned int i = 1; i < active_portion; i++)
	{
		if ((responses_pairs + i)->ground_truth_class > 0)
		{
			accumulated_false_negative += 1.0;
		}
		else
		{
			accumulated_true_negative += 1.0;
		}

		true_positive_fraction = (total_positive - accumulated_false_negative) / total_positive;
		false_positive_fraction = (total_negative - accumulated_true_negative) / total_negative;

		
		area_under_curve -= (true_positive_fraction + previous_true_positive_fraction) * (false_positive_fraction - previous_false_positive_fraction);

		fprintf(fp_roc, "%.12f %.12f %.12f\n", false_positive_fraction, true_positive_fraction, area_under_curve);

		previous_true_positive_fraction = true_positive_fraction;
		previous_false_positive_fraction = false_positive_fraction;
	}
	fclose(fp_roc);

	free(responses_pairs);

	return area_under_curve*0.5;
}



#ifdef BUILDING_PYTHON_MODULE
static PyObject* aucROCmaskedsavefile(PyObject *self, PyObject *args)
{
	PyArrayObject  *input_img;
	PyArrayObject  *groundtruth;
	PyArrayObject  *mask;
	const char * roc_curve_filename;

	npy_intp height, width, n_imgs = 1, n_channels = 1;
	
	/* Parse the input arguments to extract two numpy arrays: */
	if (!PyArg_ParseTuple(args, "O!O!O!s", &PyArray_Type, &input_img, &PyArray_Type, &groundtruth, &PyArray_Type, &mask, &roc_curve_filename))
	{
		return NULL;
	}

	if (input_img->nd > 3)
	{
		n_imgs = input_img->dimensions[0];
		n_channels = input_img->dimensions[1];
		height = input_img->dimensions[2];
		width = input_img->dimensions[3];		
	}
	else if (input_img->nd > 2)
	{
		n_imgs = input_img->dimensions[0];
		height = input_img->dimensions[1];
		width = input_img->dimensions[2];		
	}
	else
	{
		height = input_img->dimensions[0];
		width = input_img->dimensions[1];
	}
	
	char * input_img_data = input_img->data;
	char * groundtruth_data = groundtruth->data;
	char * mask_data = mask->data;
		
	npy_intp input_img_stride = input_img->strides[input_img->nd-1];
	npy_intp groundtruth_stride = groundtruth->strides[groundtruth->nd-1];
	npy_intp mask_stride = mask->strides[mask->nd-1];
	
	double auc_resp = aucROCmaskedsavefile_impl((double*)input_img_data, (double*)groundtruth_data, mask_data, height, width, n_imgs, roc_curve_filename);

	return Py_BuildValue("d", auc_resp);
}
#endif



#ifdef BUILDING_PYTHON_MODULE
static PyMethodDef computeAUCROC_methods[] = {
	{ "aucROC",	aucROC, METH_VARARGS, "compute area under the ROC curve" },
	{ "aucROCsavefile",	aucROCsavefile, METH_VARARGS, "compute area under the ROC curve and save the true and false positive fractions" },
	{ "aucROCmasked",	aucROCmasked, METH_VARARGS, "compute area under the ROC curve of the image FOV" },
	{ "aucROCmaskedsavefile",	aucROCmaskedsavefile, METH_VARARGS, "compute area under the ROC curve and save the true and false positive fractions of the image FOV" },
	{ NULL, NULL, 0, NULL }
};
#endif



#ifdef BUILDING_PYTHON_MODULE
static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"computeAUCROC",
	NULL,
	-1,
	computeAUCROC_methods,
	NULL,
	NULL,
	NULL,
	NULL
};
#endif



#ifdef BUILDING_PYTHON_MODULE
PyMODINIT_FUNC PyInit_computeAUCROC(void)
{
	PyObject *m;
	m = PyModule_Create(&moduledef);
	if (!m) {
		return NULL;
	}
	import_array();

	return m;
}
#endif

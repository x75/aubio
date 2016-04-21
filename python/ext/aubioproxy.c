#include "aubio-types.h"

PyObject *
PyAubio_CFvecToArray (fvec_t * self)
{
  npy_intp dims[] = { self->length, 1 };
  return PyArray_SimpleNewFromData (1, dims, AUBIO_NPY_SMPL, self->data);
}

int
PyAubio_ArrayToCFvec (PyObject *input, fvec_t *out) {
  if (input == NULL) {
    PyErr_SetString (PyExc_ValueError, "input array is not a python object");
    return 0;
  }
  // parsing input object into a Py_fvec
  if (PyArray_Check(input)) {

    // we got an array, convert it to an fvec
    if (PyArray_NDIM ((PyArrayObject *)input) == 0) {
      PyErr_SetString (PyExc_ValueError, "input array is a scalar");
      return 0;
    } else if (PyArray_NDIM ((PyArrayObject *)input) > 1) {
      PyErr_SetString (PyExc_ValueError,
          "input array has more than one dimensions");
      return 0;
    }

    if (!PyArray_ISFLOAT ((PyArrayObject *)input)) {
      PyErr_SetString (PyExc_ValueError, "input array should be float");
      return 0;
    } else if (PyArray_TYPE ((PyArrayObject *)input) != AUBIO_NPY_SMPL) {
      PyErr_SetString (PyExc_ValueError, "input array should be float32");
      return 0;
    }

    // vec = new_fvec (vec->length);
    // no need to really allocate fvec, just its struct member
    long length = PyArray_SIZE ((PyArrayObject *)input);
    if (length <= 0) {
      PyErr_SetString (PyExc_ValueError, "input array size should be greater than 0");
      return 0;
    }

  } else if (PyObject_TypeCheck (input, &PyList_Type)) {
    PyErr_SetString (PyExc_ValueError, "does not convert from list yet");
    return 0;
  } else {
    PyErr_SetString (PyExc_ValueError, "can only accept vector of float as input");
    return 0;
  }

  out->length = (uint_t) PyArray_SIZE ((PyArrayObject *)input);
  out->data = (smpl_t *) PyArray_GETPTR1 ((PyArrayObject *)input, 0);
  return 1;
}

PyObject *
PyAubio_CCvecToPyCvec (cvec_t * input, Py_cvec *vec) {
  vec->length = input->length;
  vec->o = input;
  // keep a reference to re-use after returning it
  Py_INCREF(vec);
  return (PyObject *)vec;
}

int
PyAubio_ArrayToCCvec (PyObject *input, cvec_t *i) {
  if (PyObject_TypeCheck (input, &Py_cvecType)) {
      //*i = *(((Py_cvec*)input)->o);
      i->norm = ((Py_cvec*)input)->o->norm;
      i->phas = ((Py_cvec*)input)->o->phas;
      i->length = ((Py_cvec*)input)->o->length;
      return 1;
  } else {
      PyErr_SetString (PyExc_ValueError, "input array should be float32");
      return 0;
  }
}

PyObject *
PyAubio_CFmatToArray (fmat_t * input)
{
  PyObject *array = NULL;
  uint_t i;
  npy_intp dims[] = { input->length, 1 };
  PyObject *concat = PyList_New (0), *tmp = NULL;
  for (i = 0; i < input->height; i++) {
    tmp = PyArray_SimpleNewFromData (1, dims, AUBIO_NPY_SMPL, input->data[i]);
    PyList_Append (concat, tmp);
    Py_DECREF (tmp);
  }
  array = PyArray_FromObject (concat, AUBIO_NPY_SMPL, 2, 2);
  Py_DECREF (concat);
  return array;
}

int
PyAubio_ArrayToCFmat (PyObject *input, fmat_t *mat) {
  uint_t i;
  if (input == NULL) {
    PyErr_SetString (PyExc_ValueError, "input array is not a python object");
    return 0;
  }
  // parsing input object into a Py_fvec
  if (PyArray_Check(input)) {

    // we got an array, convert it to an fvec
    if (PyArray_NDIM ((PyArrayObject *)input) == 0) {
      PyErr_SetString (PyExc_ValueError, "input array is a scalar");
      return 0;
    } else if (PyArray_NDIM ((PyArrayObject *)input) > 2) {
      PyErr_SetString (PyExc_ValueError,
          "input array has more than two dimensions");
      return 0;
    }

    if (!PyArray_ISFLOAT ((PyArrayObject *)input)) {
      PyErr_SetString (PyExc_ValueError, "input array should be float");
      return 0;
    } else if (PyArray_TYPE ((PyArrayObject *)input) != AUBIO_NPY_SMPL) {
      PyErr_SetString (PyExc_ValueError, "input array should be float32");
      return 0;
    }

    // no need to really allocate fvec, just its struct member
    long length = PyArray_DIM ((PyArrayObject *)input, 1);
    if (length <= 0) {
      PyErr_SetString (PyExc_ValueError, "input array dimension 1 should be greater than 0");
      return 0;
    }
    long height = PyArray_DIM ((PyArrayObject *)input, 0);
    if (height <= 0) {
      PyErr_SetString (PyExc_ValueError, "input array dimension 0 should be greater than 0");
      return 0;
    }

  } else if (PyObject_TypeCheck (input, &PyList_Type)) {
    PyErr_SetString (PyExc_ValueError, "can not convert list to fmat");
    return 0;
  } else {
    PyErr_SetString (PyExc_ValueError, "can only accept matrix of float as input");
    return 0;
  }

  if (mat->height != (uint_t)PyArray_DIM ((PyArrayObject *)input, 0)) {
    /*
    free(mat->data);
    mat->height = (uint_t)PyArray_DIM ((PyArrayObject *)input, 0);
    mat->data = (smpl_t **)malloc(sizeof(smpl_t*) * mat->height);
    */
    PyErr_Format(PyExc_ValueError, "too many rows, %d but %ld expected",
                      mat->height, PyArray_DIM ((PyArrayObject *)input, 0) );
    return 0;
  }

  mat->length = (uint_t)PyArray_DIM ((PyArrayObject *)input, 1);
  for (i=0; i< mat->height; i++) {
    mat->data[i] = (smpl_t*)PyArray_GETPTR1 ((PyArrayObject *)input, i);
  }
  return 1;
}

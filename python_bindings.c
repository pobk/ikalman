
#include <Python.h>
#include "gps.h"
#include "matrix.h"

typedef struct{
    PyObject_HEAD
    KalmanFilter filter;
    double noise;
} PyKalmanFilter;

static int py_filter_init(PyKalmanFilter *self, PyObject *args){

    double noise;
    if (!PyArg_ParseTuple(args, "d", &noise)){
        return 1;
    }
    self->noise = noise;
    self->filter = alloc_filter_velocity2d(noise);
    return 0;
}

static void py_free_filter(PyKalmanFilter *self, PyObject *args) {
    free_filter(self->filter);
}

static PyObject *py_update_velocity2d(PyKalmanFilter *self, PyObject *args) {
    double lat, lon, seconds_since_last_update;

    if (!PyArg_ParseTuple(args, "ddd", &lat, &lon, &seconds_since_last_update))
        return NULL;

    update_velocity2d(self->filter, lat, lon, seconds_since_last_update);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *py_get_lat_long(PyKalmanFilter *self, PyObject *args){

    double lat, lon;
    get_lat_long(self->filter, &lat, &lon);
    PyObject *result = Py_BuildValue("dd", lat, lon);

    return result;
}

/* Extract velocity with lat-long-per-second units from a velocity2d
   Kalman filter. */
static PyObject *py_get_velocity(PyKalmanFilter *self, PyObject *args){

    double delta_lat, delta_lon;
    get_velocity(self->filter, &delta_lat, &delta_lon);
    PyObject *result = Py_BuildValue("dd", delta_lat, delta_lon);

    return result;
}

/* Extract a bearing from a velocity2d Kalman filter.
   0 = north, 90 = east, 180 = south, 270 = west */
static PyObject *py_get_bearing(PyKalmanFilter *self, PyObject *args){

    double bearing;
    bearing = get_bearing(self->filter);
    PyObject *result = Py_BuildValue("d", bearing);

    return result;
}

/* Extract speed in miles per hour from a velocity2d Kalman filter. */
static PyObject *py_get_mph(PyKalmanFilter *self, PyObject *args){

    double mph;
    mph = get_mph(self->filter);
    PyObject *result = Py_BuildValue("d", mph);

    return result;
}

static PyMethodDef filter_methods[] = {
    { "update_velocity2d", (PyCFunction)py_update_velocity2d, METH_VARARGS, ""},
    { "get_lat_long", (PyCFunction)py_get_lat_long, METH_NOARGS, ""},
    { "get_velocity", (PyCFunction)py_get_velocity, METH_NOARGS, ""},
    { "get_bearing", (PyCFunction)py_get_bearing, METH_NOARGS, ""},
    { "get_mph", (PyCFunction)py_get_mph, METH_NOARGS, ""},
    { NULL, NULL },
};

static PyTypeObject filterType={
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "PyKalmanFilter",        /*tp_name*/
    sizeof(PyKalmanFilter),       /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)py_free_filter,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Kalman Filter",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    filter_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)py_filter_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,        /* tp_new */
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC initikalman(void) {
    PyObject *m;
    if (PyType_Ready(&filterType) < 0)
        return;

    m = Py_InitModule3("ikalman", NULL, "");
    if (m == NULL)
        return;

    Py_INCREF(&filterType);
    PyModule_AddObject(m, "filter", (PyObject *)&filterType);
}

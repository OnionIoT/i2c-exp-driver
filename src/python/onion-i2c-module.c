#include <Python.h>
#include <onion-i2c.h>

#if PY_MAJOR_VERSION < 3
#define PyLong_AS_LONG(val) PyInt_AS_LONG(val)
#define PyLong_AsLong(val) PyInt_AsLong(val)
#endif

// Macros needed for Python 3
#ifndef PyInt_Check
#define PyInt_Check			PyLong_Check
#define PyInt_FromLong		PyLong_FromLong
#define PyInt_AsLong		PyLong_AsLong
#define PyInt_Type			PyLong_Type
#endif


// static object variable for error:
static PyObject *PyOnionI2CError;


PyDoc_STRVAR(onionI2C_module_doc,
	"This module defines an object type that performs I2c transactions\n"
	"using the Linux sysfs interface\n");


typedef struct {
	PyObject_HEAD

	int 	adapterNum;
} OnionI2CObject;

// required class functions
static PyObject *
onionI2C_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	OnionI2CObject *self;
	if ((self = (OnionI2CObject *)type->tp_alloc(type, 0)) == NULL)
		return NULL;

	// setup the params to the default
	self->adapterNum	= I2C_DEFAULT_ADAPTER;

	Py_INCREF(self);
	return (PyObject *)self;
}

static int
onionI2C_init(OnionI2CObject *self, PyObject *args, PyObject *kwds)
{
	int 	adapter 	= -1;
	static 	char *kwlist[] = {"adapter", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i:__init__", kwlist, &adapter) ) {
		return -1;
	}

	if (adapter >= 0) {
		self->adapterNum	= adapter;
	}

	return 0;
}


static PyObject *
onionI2C_close(OnionI2CObject *self)
{
	// reset the params
	self->adapterNum	= -1;

	Py_INCREF(Py_None);
	return Py_None;
}

static void
onionI2C_dealloc(OnionI2CObject *self)
{
	PyObject *ref = onionI2C_close(self);
	Py_XDECREF(ref);

	Py_TYPE(self)->tp_free((PyObject *)self);
}



/*
 * 	Functions being mapped to object functions
 */

static char *wrmsg_list0 	= "Empty argument list.";
static char *wrmsg_buffer 	= "Attempting to read more bytes than I2C max buffer: %d.";
static char *wrmsg_i2c 		= "I2C transaction failed.";
static char *wrmsg_val 		= "Non-Int/Long value in arguments: %x.";


PyDoc_STRVAR(onionI2C_setVerbosity_doc,
	"setVerbosity(level) -> None\n\n"
	"Set the verbosity for the object (-1 to 2).\n");

static PyObject *
onionI2C_setVerbosity(OnionI2CObject *self, PyObject *args)
{
	int 		verbose;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &verbose) ) {
		return NULL;
	}

	// set the verbosity level
	onionSetVerbosity(verbose);
	

	Py_INCREF(Py_None);
	return Py_None;
}


PyDoc_STRVAR(onionI2C_readBytes_doc,
	"readBytes(deviceAddr, addr, numBytes) -> [values]\n\n"
	"Read 'numBytes' bytes from address 'addr' on I2C device with 'deviceAddr' address.\n");

static PyObject *
onionI2C_readBytes(OnionI2CObject *self, PyObject *args)
{
	int 		status, devAddr, addr, bytes, i;
	uint8_t		buffer[I2C_BUFFER_SIZE];
	PyObject	*list;
	char		wrmsg_text[4096];


	// parse the arguments
	if (!PyArg_ParseTuple(args, "iii", &devAddr, &addr, &bytes) ) {
		return NULL;
	}

	// check the number of bytes
	if (bytes > I2C_BUFFER_SIZE) {
		snprintf(wrmsg_text, sizeof (wrmsg_text) - 1, wrmsg_buffer, I2C_BUFFER_SIZE);
		PyErr_SetString(PyExc_OverflowError, wrmsg_text);
		return NULL;
	}

	// perform the read
	status	= i2c_read	(	self->adapterNum, 
							devAddr, 
							addr, 
							buffer,
							bytes
						);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	// build the python object to be returned from the buffer
	list = PyList_New(bytes);

	for (i = 0; i < bytes; i++) {
		PyObject *val = Py_BuildValue("l", (long)buffer[i]);
		PyList_SET_ITEM(list, i, val);
	}


	return list;
}

PyDoc_STRVAR(onionI2C_writeBytes_doc,
	"writeBytes(deviceAddr, addr, [values]) -> None\n\n"
	"Write bytes from 'values' list to address 'addr' on I2C device with 'deviceAddr' address.\n");

static PyObject *
onionI2C_writeBytes(OnionI2CObject *self, PyObject *args)
{
	int 		status, devAddr, addr, bytes, i;
	uint8_t		buffer[I2C_BUFFER_SIZE];
	PyObject	*list;
	char		wrmsg_text[4096];


	// parse the arguments
	if (!PyArg_ParseTuple(args, "iiO", &devAddr, &addr, &list) ) {
		return NULL;
	}

	// check the number of bytes
	bytes 	= PyList_Size(list);
	if (bytes <= 0) {
		PyErr_SetString(PyExc_TypeError, wrmsg_list0);
		return NULL;
	}

	if (bytes > I2C_BUFFER_SIZE) {
		snprintf(wrmsg_text, sizeof (wrmsg_text) - 1, wrmsg_buffer, I2C_BUFFER_SIZE);
		PyErr_SetString(PyExc_OverflowError, wrmsg_text);
		return NULL;
	}

	// populate the values (by iterating through the list)
	for (i = 0; i < bytes; i++) {
		PyObject *val = PyList_GET_ITEM(list, i);
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(val)) {
			buffer[i] = (uint8_t)PyInt_AS_LONG(val);
		} else
#endif
		{
			if (PyLong_Check(val)) {
				buffer[i] = (uint8_t)PyLong_AS_LONG(val);
			} else {
				snprintf(wrmsg_text, sizeof (wrmsg_text) - 1, wrmsg_val, val);
				PyErr_SetString(PyExc_TypeError, wrmsg_text);
				return NULL;
			}
		}
	}

	// perform the write
	status 	= i2c_writeBuffer	(	self->adapterNum, 
									devAddr,
									addr, 
									buffer, 
									bytes
								);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}


	Py_INCREF(Py_None);
	return Py_None;
}

PyDoc_STRVAR(onionI2C_writeByte_doc,
	"writeByte(deviceAddr, addr, value) -> None\n\n"
	"Write a single byte, 'value' to address 'addr' on I2C device with 'deviceAddr' address.\n");

static PyObject *
onionI2C_writeByte(OnionI2CObject *self, PyObject *args)
{
	int 		status, devAddr, addr, bytes, value;
	uint8_t		buffer[I2C_BUFFER_SIZE];
	PyObject	*list;
	char		wrmsg_text[4096];


	// parse the arguments
	if (!PyArg_ParseTuple(args, "iii", &devAddr, &addr, &value) ) {
		return NULL;
	}

	// set number of bytes and populate the buffer
	bytes 		= 1;
	buffer[0] 	= value;

	// perform the write
	status 	= i2c_writeBuffer	(	self->adapterNum, 
									devAddr,
									addr, 
									buffer, 
									bytes
								);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}


	Py_INCREF(Py_None);
	return Py_None;
}

PyDoc_STRVAR(onionI2C_write_doc,
	"write(deviceAddr, [values]) -> None\n\n"
	"Write a list of bytes 'values' to I2C device with 'deviceAddr' address.\n");

static PyObject *
onionI2C_write(OnionI2CObject *self, PyObject *args)
{
	int 		status, devAddr, bytes, i;
	uint8_t		buffer[I2C_BUFFER_SIZE];
	PyObject	*list;
	char		wrmsg_text[4096];


	// parse the arguments
	if (!PyArg_ParseTuple(args, "iO", &devAddr, &list) ) {
		return NULL;
	}

	// check the number of bytes
	bytes 	= PyList_Size(list);
	if (bytes <= 0) {
		PyErr_SetString(PyExc_TypeError, wrmsg_list0);
		return NULL;
	}

	if (bytes > I2C_BUFFER_SIZE) {
		snprintf(wrmsg_text, sizeof (wrmsg_text) - 1, wrmsg_buffer, I2C_BUFFER_SIZE);
		PyErr_SetString(PyExc_OverflowError, wrmsg_text);
		return NULL;
	}

	// populate the values (by iterating through the list)
	for (i = 0; i < bytes; i++) {
		PyObject *val = PyList_GET_ITEM(list, i);
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(val)) {
			buffer[i] = (uint8_t)PyInt_AS_LONG(val);
		} else
#endif
		{
			if (PyLong_Check(val)) {
				buffer[i] = (uint8_t)PyLong_AS_LONG(val);
			} else {
				snprintf(wrmsg_text, sizeof (wrmsg_text) - 1, wrmsg_val, val);
				PyErr_SetString(PyExc_TypeError, wrmsg_text);
				return NULL;
			}
		}
	}

	// perform the write (just writing the buffer, no address argument)
	status 	= _i2c_writeBuffer	(	self->adapterNum, 
									devAddr,
									buffer, 
									bytes
								);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}


	Py_INCREF(Py_None);
	return Py_None;
}


/*
 * 	Define the get and set functions for the parameters
 */

// pass in a python value, get a C int back
//	returns -1 if conversion failed for whatever reason
static int
onionI2C_convertPyValToInt (PyObject *val)
{
	int 	cValue;

	// check for a valid value
	if (val == NULL) {
		PyErr_SetString(PyExc_TypeError,
			"Cannot delete attribute");
		return -1;
	}

	// convert the python value
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(val)) {
		cValue = PyInt_AS_LONG(val);
	} else
#endif
	{
		if (PyLong_Check(val)) {
			cValue = PyLong_AS_LONG(val);
		} else {
			PyErr_SetString(PyExc_TypeError,
				"The attribute must be an integer");
			return -1;
		}
	}

	return 	cValue;
}

// adapterNum 
static PyObject *
onionI2C_get_adapterNum(OnionI2CObject *self, void *closure)
{
	// create a python value from the integer
	PyObject *result = Py_BuildValue("i", self->adapterNum);
	Py_INCREF(result);
	return result;
}

static int
onionI2C_set_adapterNum(OnionI2CObject *self, PyObject *val, void *closure)
{
	uint32_t value;

	// convert the python value
	value 	= onionI2C_convertPyValToInt(val);

	if (value != -1) {
		self->adapterNum= value;
		return 0;
	}
	
	return -1;
}

static PyGetSetDef onionI2C_getset[] = {
	{"adapter", (getter)onionI2C_get_adapterNum, (setter)onionI2C_set_adapterNum,
			"I2C adapter number\n"},
	
	{NULL} 		/* Sentinel */
};


////// Python Module Setup //////
/*
 * 	Bind Python function names to the C functions
 */
PyDoc_STRVAR(OnionI2CObjectType_doc,
	"onionI2C([adapter]) -> I2C\n\n"
	"Return an object that can be used to communicate through a\n"
	"I2C device interface.\n");

static PyMethodDef onionI2C_methods[] = {
	{"setVerbosity", 	(PyCFunction)onionI2C_setVerbosity, 	METH_VARARGS, 		onionI2C_setVerbosity_doc},

	{"readBytes", 		(PyCFunction)onionI2C_readBytes, 		METH_VARARGS, 		onionI2C_readBytes_doc},

	{"writeBytes", 		(PyCFunction)onionI2C_writeBytes, 		METH_VARARGS, 		onionI2C_writeBytes_doc},
	{"writeByte", 		(PyCFunction)onionI2C_writeByte, 		METH_VARARGS, 		onionI2C_writeByte_doc},
	{"write", 			(PyCFunction)onionI2C_write, 			METH_VARARGS, 		onionI2C_write_doc},

	{NULL, NULL}	/* Sentinel */
};


/*
 * 	Python object bindings
 */
static PyTypeObject OnionI2CObjectType = {
#if PY_MAJOR_VERSION >= 3
	PyVarObject_HEAD_INIT(NULL, 0)
#else
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size */
#endif
	"OnionI2C",			/* tp_name */
	sizeof(OnionI2CObject),		/* tp_basicsize */
	0,				/* tp_itemsize */
	(destructor)onionI2C_dealloc,	/* tp_dealloc */
	0,				/* tp_print */
	0,				/* tp_getattr */
	0,				/* tp_setattr */
	0,				/* tp_compare */
	0,				/* tp_repr */
	0,				/* tp_as_number */
	0,				/* tp_as_sequence */
	0,				/* tp_as_mapping */
	0,				/* tp_hash */
	0,				/* tp_call */
	0,				/* tp_str */
	0,				/* tp_getattro */
	0,				/* tp_setattro */
	0,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	OnionI2CObjectType_doc,		/* tp_doc */
	0,				/* tp_traverse */
	0,				/* tp_clear */
	0,				/* tp_richcompare */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter */
	0,				/* tp_iternext */
	onionI2C_methods,			/* tp_methods */
	0,				/* tp_members */
	onionI2C_getset,			/* tp_getset */
	0,				/* tp_base */
	0,				/* tp_dict */
	0,				/* tp_descr_get */
	0,				/* tp_descr_set */
	0,				/* tp_dictoffset */
	(initproc)onionI2C_init,		/* tp_init */
	0,				/* tp_alloc */
	onionI2C_new,			/* tp_new */
};

static PyMethodDef onionI2C_module_methods[] = {
	{NULL}	/* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"onionI2C",
	onionI2C_module_doc,
	-1,
	onionI2C_module_methods,
	NULL,
	NULL,
	NULL,
	NULL,
};
#else
#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
#endif



/*
 * 	Python calls this to initialize this module
 */
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_onionI2C(void)
#else
void initonionI2C(void)
#endif
{
	PyObject *m;

	if (PyType_Ready(&OnionI2CObjectType) < 0)
#if PY_MAJOR_VERSION >= 3
		return NULL;
#else
		return;
#endif

#if PY_MAJOR_VERSION >= 3
	m = PyModule_Create(&moduledef);
#else
	m = Py_InitModule3("onionI2C", onionI2C_methods, onionI2C_module_doc);
#endif
    if (m == NULL)
#if PY_MAJOR_VERSION >= 3
		return NULL;
#else
		return;
#endif

    Py_INCREF(&OnionI2CObjectType);
	PyModule_AddObject(m, "OnionI2C", (PyObject *)&OnionI2CObjectType);


    PyOnionI2CError = PyErr_NewException("onionI2C.error", NULL, NULL);
    Py_INCREF(PyOnionI2CError);
    PyModule_AddObject(m, "error", PyOnionI2CError);

#if PY_MAJOR_VERSION >= 3
	return m;
#endif
}


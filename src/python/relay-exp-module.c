#include <Python.h>
#include <relay-exp.h>


// static object variable for error:
static PyObject *PyRelayExpError;

static char *wrmsg_i2c 		= "I2C transaction failed.";


/*
 * 	Python Analogue for setting verbosity
 */
static PyObject* pyRelaySetVerbosity(PyObject* self, PyObject* args)
{
	int 	verbosity;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &verbosity) ) {
		return NULL;
	}

	// make the call
	onionSetVerbosity (verbosity);

	return Py_BuildValue("i", 0);
}

/*
 * 	relayDriverInit() Python Analogue
 */
static PyObject* pyRelayDriverInit(PyObject* self, PyObject* args)
{
	int 	status, addr;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &addr) ) {
		return NULL;
	}

	// make the relay-exp call
	status 	= relayDriverInit (addr);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	relayCheckInit() Python Analogue
 */
static PyObject* pyRelayCheckInit(PyObject* self, PyObject* args)
{
	int 	status, addr, bInit;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &addr) ) {
		return NULL;
	}

	// make the relay-exp call
	status 	= relayCheckInit (addr, &bInit);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", bInit); 
}

/*
 * 	relaySetChannel() Python Analogue
 */
static PyObject* pyRelaySetChannel(PyObject* self, PyObject* args)
{
	int 	status, addr, channel, state;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "iii", &addr, &channel, &state) ) {
		return NULL;
	}

	// make the relay-exp call
	status 	= relaySetChannel (addr, channel, state);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	relaySetAllChannels() Python Analogue
 */
static PyObject* pyRelaySetAllChannels(PyObject* self, PyObject* args)
{
	int 	status, addr, state;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "ii", &addr, &state) ) {
		return NULL;
	}

	// make the relay-exp call
	status 	= relaySetAllChannels (addr, state);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}


////// Python Module Setup //////
/*
 * 	Bind Python function names to the C functions
 */
static PyMethodDef pyRelayExpMethods[] = {
	{"setVerbosity", 	pyRelaySetVerbosity, 	METH_VARARGS},
	{"driverInit", 		pyRelayDriverInit, 		METH_VARARGS},
	{"checkInit", 		pyRelayCheckInit, 		METH_VARARGS},
	{"setChannel", 		pyRelaySetChannel, 		METH_VARARGS},
	{"setAllChannels", 	pyRelaySetAllChannels, 	METH_VARARGS},
	{NULL, NULL}	/* Sentinel */
};

/*
 * 	Python calls this to initialize this module
 */
void initrelayExp()
{
	PyObject *m;

    m = Py_InitModule("relayExp", pyRelayExpMethods);
    if (m == NULL)
        return;

    PyRelayExpError = PyErr_NewException("relayExp.error", NULL, NULL);
    Py_INCREF(PyRelayExpError);
    PyModule_AddObject(m, "error", PyRelayExpError);
}
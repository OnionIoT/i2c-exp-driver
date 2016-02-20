#include <Python.h>
#include <pwm-exp.h>


// static object variable for error:
static PyObject *PyPwmExpError;

static char *wrmsg_i2c 		= "I2C transaction failed.";


/*
 * 	Python Analogue for setting verbosity
 */
static PyObject* pyPwmSetVerbosity(PyObject* self, PyObject* args)
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
 * 	pwmDriverInit() Python Analogue
 */
static PyObject* pyPwmDriverInit(PyObject* self, PyObject* args)
{
	int 	status;

	// make the pwm-exp call
	status 	= pwmDriverInit ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	relayCheckInit() Python Analogue
 */
static PyObject* pyPwmCheckInit(PyObject* self, PyObject* args)
{
	int 	status, bInit;

	// make the pwm-exp call
	status 	= pwmCheckInit (&bInit);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", bInit); 
}

/*
 * 	pwmDisableChip() Python Analogue
 */
static PyObject* pyPwmDisableChip(PyObject* self, PyObject* args)
{
	int 	status;

	// make the pwm-exp call
	status 	= pwmDisableChip ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	pwmSetFrequency() Python Analogue
 */
static PyObject* pyPwmSetFrequency(PyObject* self, PyObject* args)
{
	int 	status;
	float 	freq;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "f", &freq) ) {
		return NULL;
	}

	// make the pwm-exp call
	status 	= pwmSetFrequency (freq);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	relaySetAllChannels() Python Analogue
 */
static PyObject* pyPwmSetupDriver(PyObject* self, PyObject* args)
{
	int 	status, channel;
	float 	duty, delay;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "iff", &channel, &duty, &delay) ) {
		return NULL;
	}

	// make the pwm-exp call
	status 	= pwmSetupDriver (channel, duty, delay);

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
static PyMethodDef pyPwmExpMethods[] = {
	{"setVerbosity", 	pyPwmSetVerbosity, 		METH_VARARGS},
	{"driverInit", 		pyPwmDriverInit, 		METH_VARARGS},
	{"checkInit", 		pyPwmCheckInit, 		METH_VARARGS},
	{"disableChip", 	pyPwmDisableChip, 		METH_VARARGS},
	{"setFrequency",	pyPwmSetFrequency, 		METH_VARARGS},
	{"setupDriver", 	pyPwmSetupDriver, 		METH_VARARGS},
	{NULL, NULL}	/* Sentinel */
};

/*
 * 	Python calls this to initialize this module
 */
void initpwmExp()
{
	PyObject *m;

    m = Py_InitModule("pwmExp", pyPwmExpMethods);
    if (m == NULL)
        return;

    PyPwmExpError = PyErr_NewException("pwmExp.error", NULL, NULL);
    Py_INCREF(PyPwmExpError);
    PyModule_AddObject(m, "error", PyPwmExpError);
}


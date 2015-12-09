#include <Python.h>
#include <pwm-exp.h>


// static object variable for error:
static PyObject *PyPwmExpError;


/*
 * 	Python Analogue for setting verbosity
 */
static PyObject* pyPwmSetVerbosity(PyObject* self, PyObject* args)
{
	int 	verbosity;

	// parse the address
	PyArg_ParseTuple(args, "i", &verbosity);

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

	return Py_BuildValue("i", status);
}

/*
 * 	pwmSetFrequency() Python Analogue
 */
static PyObject* pyPwmSetFrequency(PyObject* self, PyObject* args)
{
	int 	status;
	float 	freq;

	// parse the address, channel, and state
	PyArg_ParseTuple(args, "f", &freq);

	// make the pwm-exp call
	status 	= pwmSetFrequency (freq);

	return Py_BuildValue("i", status);
}

/*
 * 	relaySetAllChannels() Python Analogue
 */
static PyObject* pyPwmSetupDriver(PyObject* self, PyObject* args)
{
	int 	status, channel;
	float 	duty, delay;

	// parse the address, channel, and state
	PyArg_ParseTuple(args, "iff", &channel, &duty, &delay);

	// make the pwm-exp call
	status 	= pwmSetupDriver (channel, duty, delay);

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


#include <Python.h>
#include <relay-exp.h>


// static object variable for error:
static PyObject *PyRelayExpError;

/*
 * 	relayDriverInit() Python Analogue
 */
static PyObject* pyRelayDriverInit(PyObject* self, PyObject* args)
{
	int 	status, addr;

	// parse the address
	PyArg_ParseTuple(args, "i", &addr);

	// make the relay-exp call
	status 	= relayDriverInit (addr);

	return Py_BuildValue("i", status);
}

/*
 * 	relayCheckInit() Python Analogue
 */
static PyObject* pyRelayCheckInit(PyObject* self, PyObject* args)
{
	int 	status, addr, bInit;

	// parse the address
	PyArg_ParseTuple(args, "i", &addr);

	// make the relay-exp call
	status 	= relayCheckInit (addr, &bInit);

	return Py_BuildValue("i", bInit); 
}

/*
 * 	relaySetChannel() Python Analogue
 */
static PyObject* pyRelaySetChannel(PyObject* self, PyObject* args)
{
	int 	status, addr, channel, state;

	// parse the address, channel, and state
	PyArg_ParseTuple(args, "iii", &addr, &channel, &state);

	// make the relay-exp call
	status 	= relaySetChannel (addr, channel, state);

	return Py_BuildValue("i", status);
}

/*
 * 	relaySetAllChannels() Python Analogue
 */
static PyObject* pyRelaySetAllChannels(PyObject* self, PyObject* args)
{
	int 	status, addr, state;

	// parse the address, channel, and state
	PyArg_ParseTuple(args, "ii", &addr, &state);

	// make the relay-exp call
	status 	= relaySetAllChannels (addr, state);

	return Py_BuildValue("i", status);
}

/*
 * 	Bind Python function names to the C functions
 */
static PyMethodDef pyRelayExpMethods[] = {
  {"relayDriverInit", 		pyRelayDriverInit, 		METH_VARARGS},
  {"relayCheckInit", 		pyRelayCheckInit, 		METH_VARARGS},
  {"relaySetChannel", 		pyRelaySetChannel, 		METH_VARARGS},
  {"relaySetAllChannels", 	pyRelaySetAllChannels, 	METH_VARARGS},
  {NULL, NULL}	/* Sentinel */
};

/*
 * 	Python calls this to initialize this module
 */
void initrelayexp()
{
	PyObject *m;

    m = Py_InitModule("relayexp", pyRelayExpMethods);
    if (m == NULL)
        return;

    PyRelayExpError = PyErr_NewException("relayexp.error", NULL, NULL);
    Py_INCREF(PyRelayExpError);
    PyModule_AddObject(m, "error", PyRelayExpError);
}
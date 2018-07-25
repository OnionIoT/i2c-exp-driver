#include <Python.h>
#include <oled-exp.h>


// static object variable for error:
static PyObject *PyOledExpError;

static char *wrmsg_i2c 		= "I2C transaction failed.";
static char *wrmsg_file 	= "Reading image file failed.";


/*
 * 	Python Analogue for setting verbosity
 */
static PyObject* pyOledSetVerbosity(PyObject* self, PyObject* args)
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
 * 	oledDriverInit() Python Analogue
 */
static PyObject* pyOledDriverInit(PyObject* self, PyObject* args)
{
	int 	status;

	// make the oled-exp call
	status 	= oledDriverInit ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledClear() Python Analogue
 */
static PyObject* pyOledClear(PyObject* self, PyObject* args)
{
	int 	status;

	// make the oled-exp call
	status 	= oledClear ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status); 
}


////// Configuration Functions //////
/*
 * 	oledSetDisplayPower() Python Analogue
 */
static PyObject* pyOledSetDisplayPower(PyObject* self, PyObject* args)
{
	int 	status, bOn;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &bOn) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetDisplayPower (bOn);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetDisplayMode() Python Analogue
 */
static PyObject* pyOledSetDisplayMode(PyObject* self, PyObject* args)
{
	int 	status, bInvert;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &bInvert) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetDisplayMode (bInvert);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetBrightness() Python Analogue
 */
static PyObject* pyOledSetBrightness(PyObject* self, PyObject* args)
{
	int 	status, brightness;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &brightness) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetBrightness (brightness);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetDim() Python Analogue
 */
static PyObject* pyOledSetDim(PyObject* self, PyObject* args)
{
	int 	status, bDim;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &bDim) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetDim (bDim);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetMemoryMode() Python Analogue
 */
static PyObject* pyOledSetMemoryMode(PyObject* self, PyObject* args)
{
	int 	status, mode;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &mode) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetMemoryMode (mode);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}


/*
 * 	oledSetCursor() Python Analogue
 */
static PyObject* pyOledSetCursor(PyObject* self, PyObject* args)
{
	int 	status, row, col;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "ii", &row, &col) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetCursor (row, col);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetCursorByPixel() Python Analogue
 */
static PyObject* pyOledSetCursorByPixel(PyObject* self, PyObject* args)
{
	int 	status, row, pixel;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "ii", &row, &pixel) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetCursorByPixel (row, pixel);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetColumnAddressing() Python Analogue
 */
static PyObject* pyOledSetColumnAddressing(PyObject* self, PyObject* args)
{
	int 	status, startPixel, endPixel;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "ii", &startPixel, &endPixel) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledSetColumnAddressing (startPixel, endPixel);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetTextColumns() Python Analogue
 */
static PyObject* pyOledSetTextColumns(PyObject* self, PyObject* args)
{
	int 	status, startPixel, endPixel;

	// make the oled-exp call
	status 	= oledSetTextColumns ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledSetImageColumns() Python Analogue
 */
static PyObject* pyOledSetImageColumns(PyObject* self, PyObject* args)
{
	int 	status, startPixel, endPixel;

	// make the oled-exp call
	status 	= oledSetImageColumns ();

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}



////// Display Functions //////
/*
 * 	oledWriteChar() Python Analogue
 */
static PyObject* pyOledWriteChar(PyObject* self, PyObject* args)
{
	int 		status;
	const char 	*msg;
	char 		c;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "s", &msg) ) {
		return NULL;
	}

	c 	= (char)msg[0];

	// make the oled-exp call
	status 	= oledWriteChar (c);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledWrite() Python Analogue
 */
static PyObject* pyOledWrite(PyObject* self, PyObject* args)
{
	int 		status;
	const char 	*msg;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "s", &msg) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledWrite ((char*)msg);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledWrite() Python Analogue
 */
static PyObject* pyOledWriteByte(PyObject* self, PyObject* args)
{
	int 		status;
	int 		byte;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "i", &byte) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledWriteByte (byte);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 *	DIFFERENT FROM C LIB:
 * 	oledDrawFromFile() 
 */
static PyObject* pyOledDrawFromFile(PyObject* self, PyObject* args)
{
	int 		status;
	uint8_t		*buffer;
	const char 	*filename;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "s", &filename) ) {
		return NULL;
	}

	// allocate memory for the buffer
	buffer 	= malloc(OLED_EXP_WIDTH*OLED_EXP_HEIGHT/8 * sizeof *buffer);

	// read LCD data from file
	status 	= oledReadLcdFile ((char*)filename, buffer);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_file);
		return NULL;
	}

	// draw data onto screen
	status 	= oledDraw (buffer, OLED_EXP_WIDTH*OLED_EXP_HEIGHT/8);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}


	return Py_BuildValue("i", status);
}

// LAZAR: implement draw from buffer functions




////// Scrolling Functions //////
/*
 * 	oledScroll() Python Analogue
 */
static PyObject* pyOledScroll(PyObject* self, PyObject* args)
{
	int 	status;
	int 	direction, scrollSpeed, startPage, stopPage;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "iiii", &direction, &scrollSpeed, &startPage, &stopPage) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledScroll (direction, scrollSpeed, startPage, stopPage);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledScrollDiagonal() Python Analogue
 */
static PyObject* pyOledScrollDiagonal(PyObject* self, PyObject* args)
{
	int 	status;
	int 	direction, scrollSpeed, fixedRows, scrollRows, verticalOffset, startPage, stopPage;

	// parse the arguments
	if (!PyArg_ParseTuple(args, "iiiiiii", &direction, &scrollSpeed, &fixedRows, &scrollRows, &verticalOffset, &startPage, &stopPage) ) {
		return NULL;
	}

	// make the oled-exp call
	status 	= oledScrollDiagonal (direction, scrollSpeed, fixedRows, scrollRows, verticalOffset, startPage, stopPage);

	if (status != EXIT_SUCCESS) {
		PyErr_SetString(PyExc_IOError, wrmsg_i2c);
		return NULL;
	}

	return Py_BuildValue("i", status);
}

/*
 * 	oledScrollStop() Python Analogue
 */
static PyObject* pyOledScrollStop(PyObject* self, PyObject* args)
{
	int 	status;

	// make the oled-exp call
	status 	= oledScrollStop ();

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
static PyMethodDef pyOledExpMethods[] = {
	{"setVerbosity", 		pyOledSetVerbosity, 			METH_VARARGS},
	
	{"driverInit", 			pyOledDriverInit, 				METH_VARARGS},
	
	{"clear", 				pyOledClear, 					METH_VARARGS},
	{"setDisplayPower", 	pyOledSetDisplayPower, 			METH_VARARGS},
	{"setDisplayMode", 		pyOledSetDisplayMode, 			METH_VARARGS},
	{"setBrightness", 		pyOledSetBrightness, 			METH_VARARGS},
	{"setDim", 				pyOledSetDim, 					METH_VARARGS},
	{"setMemoryMode", 		pyOledSetMemoryMode, 			METH_VARARGS},
		
	{"setCursor", 			pyOledSetCursor, 				METH_VARARGS},
	{"setCursorByPixel", 	pyOledSetCursorByPixel,			METH_VARARGS},
		
	{"setColumnAddressing", pyOledSetColumnAddressing, 		METH_VARARGS},
	{"setImageColumns", 	pyOledSetImageColumns, 			METH_VARARGS},
	{"setTextColumns", 		pyOledSetTextColumns, 			METH_VARARGS},
		
	{"writeChar", 			pyOledWriteChar, 				METH_VARARGS},
	{"write", 				pyOledWrite, 					METH_VARARGS},
	{"writeByte", 			pyOledWriteByte,				METH_VARARGS},
		
	{"drawFromFile", 		pyOledDrawFromFile, 			METH_VARARGS},
		
	{"scroll", 				pyOledScroll, 					METH_VARARGS},
	{"scrollDiagonal", 		pyOledScrollDiagonal, 			METH_VARARGS},
	{"scrollStop", 			pyOledScrollStop, 				METH_VARARGS},
	
	{NULL, NULL}	/* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef oledExp = {
	PyModuleDef_HEAD_INIT,
	"relayExp",
	"Onion OLED Expansio Module",
	-1,
	pyOledExpMethods,
	NULL, NULL, NULL, NULL
};
#endif

PyObject * PyInit_oledExp(void)
{
#if PY_MAJOR_VERSION >= 3
	return PyModule_Create(&oledExp);
#else
    return Py_InitModule("oledExp", pyOledExpMethods);
#endif
}

/*
 * 	Python calls this to initialize this module
 */
void initoledExp()
{
	PyObject *m;

    m = PyInit_oledExp();
    if (m == NULL)
        return;

    PyOledExpError = PyErr_NewException("oledExp.error", NULL, NULL);
    Py_INCREF(PyOledExpError);
    PyModule_AddObject(m, "error", PyOledExpError);
}

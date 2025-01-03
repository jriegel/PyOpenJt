

#include <Python.h>
#include <string>

static PyObject *PyOpenJtError;


// forwards of the addition of class handlers
void addJtFileClass(PyObject *module);





static PyObject *
_getAttribute(PyObject *, PyObject *args)
{
   const char *type = nullptr;
   
   if (!PyArg_ParseTuple(args, "s", &type))
        return NULL;

   const std::string attr(type);

   if (attr == "test")
       return PyLong_FromLong(0);

    return Py_None;
}



static PyMethodDef PyOpenJt_Methods[] = {
    { "__getattr__", _getAttribute, METH_VARARGS,
    "internal methode for attribute retrivel" },
#if 0
    { "setEnv", _setEnv, METH_VARARGS,
    "set a environment key." },
    { "getEnv", _getEnv, METH_VARARGS,
    "get a environment key." },
    { "init", _init, METH_VARARGS,
    "Initialize the BCA system." },
#endif

    { NULL, NULL, 0, NULL }        /* Sentinel */
};

static struct PyModuleDef PyOpenJt_Module = {
    PyModuleDef_HEAD_INIT,
    "PyOpenJt",   /* name of module */
    NULL , /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
              or -1 if the module keeps state in global variables. */
	PyOpenJt_Methods, 
	NULL, NULL, NULL, NULL 
};

PyMODINIT_FUNC
PyInit_PyOpenJt(void)
{
    PyObject *module;

    module = PyModule_Create(&PyOpenJt_Module);
    if (module == NULL)
        return NULL;
    PyOpenJtError = PyErr_NewException("3DLife.error", NULL, NULL);
    Py_INCREF(PyOpenJtError);
    PyModule_AddObject(module, "error", PyOpenJtError);

    // add the JT file class
    addJtFileClass(module);

 
    return module; 
}



#include <Python.h>
#include <string>

#include <JtData_Model.hxx>
#include <JtNode_Base.hxx>
#include <JtNode_Partition.hxx>


const char* JtFile_doc = "Object representing an open JT file";
extern PyTypeObject JtFileClassType;
PyObject *JtFile_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

typedef struct {
    PyObject_HEAD;
    std::string FileName;
    Handle(JtNode_Partition) PartitionNode;
    Handle(JtData_Model) rootModel;
} JtFileObject;




static PyObject *
JtFile_getFileName(JtFileObject *self, void *)
{
    return  PyUnicode_FromString(self->FileName.c_str());
}

int JtFile_setFileName(JtFileObject* self, PyObject* value, void* closure)
{
    self->FileName = PyUnicode_AsUTF8AndSize(value,0);
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_ValueError, "Parameter have to be Unicode string!");
        return -1;
    }

    return 0;
}


static PyGetSetDef JtFile_getseters[] = {
    { "FileName",(getter)JtFile_getFileName, (setter) JtFile_setFileName , "File Path of the JT file",   NULL }, 
    { NULL }  /* Sentinel */
};



static int
JtFile_init(JtFileObject* self, PyObject* args, PyObject*)
{
    char* value;
    if (!PyArg_ParseTuple(args, "s", &value)) {
        return -1;
    }

    self->FileName = value;

    return 0;
}

static PyObject*
JtFile_open(JtFileObject* self, PyObject* args, PyObject*)
{

    Py_BEGIN_ALLOW_THREADS;

    TCollection_ExtendedString aFileName(self->FileName.c_str(), Standard_True);

    self->rootModel = new JtData_Model(aFileName);

    if (!self->rootModel.IsNull())
        self->PartitionNode = self->rootModel->Init();

    Py_END_ALLOW_THREADS;

    if (self->rootModel.IsNull() || self->PartitionNode.IsNull()) {
        Py_INCREF(Py_False);
        return Py_False;
    }

    Py_INCREF(Py_True);
    return Py_True;
}

static PyMethodDef JtFile_methods[] = {
    { "open", (PyCFunction)JtFile_open, METH_VARARGS | METH_KEYWORDS ,"Open the file and read the LSG" },
    { NULL }  /* Sentinel */
};

static void
JtFile_dealloc(JtFileObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *
JtFile_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    kwds; args;
    JtFileObject *self;

    self = (JtFileObject *)type->tp_alloc(type, 0);
    
    return (PyObject *)self;
}

PyTypeObject JtFileClassType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyopenjt.JtFile",      /* tp_name */
    sizeof(JtFileObject),   /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)JtFile_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    JtFile_doc,                 /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    JtFile_methods,             /* tp_methods */
    0,                         /* tp_members */
    JtFile_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)JtFile_init,      /* tp_init */
    0,                         /* tp_alloc */
    JtFile_new,                 /* tp_new */
};


void addJtFileClass(PyObject * module)
{
	JtFileClassType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&JtFileClassType) < 0)
		return;
	Py_INCREF(&JtFileClassType);
	PyModule_AddObject(module, "JtFile", (PyObject *)&JtFileClassType);
}



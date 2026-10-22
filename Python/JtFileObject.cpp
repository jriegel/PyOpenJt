
#include <Python.h>
#include <string>
#include <sstream>      // std::stringstream
#include <filesystem>
#include <memory>
#include <new>

#include <JtData_Model.hxx>
#include <JtNode_Base.hxx>
#include <JtNode_Partition.hxx>

#include <JtData2Json.h>
#include <JtGltfMesh.h>

using namespace std;

const char* JtFile_doc = "Object representing an open JT file";

typedef struct {
    PyObject_HEAD;
    std::string FileName;
    std::shared_ptr<JtNode_Partition> PartitionNode;
    std::shared_ptr<JtData_Model> rootModel;
} JtFileObject;




static PyObject *
JtFile_getFileName(JtFileObject *self, void *)
{
    return  PyUnicode_FromString(self->FileName.c_str());
}

int JtFile_setFileName(JtFileObject* self, PyObject* value, void* )
{
    PyObject* encoded = PyUnicode_AsEncodedString(value, "utf-8", "strict");
    if (encoded == nullptr) {
        PyErr_SetString(PyExc_ValueError, "Parameter have to be Unicode string!");
        return -1;
    }

    char* fileName;
    Py_ssize_t fileNameSize;
    if (PyBytes_AsStringAndSize(encoded, &fileName, &fileNameSize) < 0) {
        Py_DECREF(encoded);
        return -1;
    }

    self->FileName.assign(fileName, static_cast<size_t>(fileNameSize));
    Py_DECREF(encoded);
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
JtFile_open(JtFileObject* self, PyObject* , PyObject*)
{

    Py_BEGIN_ALLOW_THREADS;

    std::wstring aFileName = std::filesystem::u8path(self->FileName).wstring();

    self->rootModel = std::make_shared<JtData_Model>(aFileName);

    if (self->rootModel)
        self->PartitionNode = self->rootModel->Init();

    Py_END_ALLOW_THREADS;

    if (!self->rootModel || !self->PartitionNode) {
        Py_INCREF(Py_False);
        return Py_False;
    }

    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject*
JtFile_json(JtFileObject* self, PyObject* , PyObject*)
{
    if (!self->rootModel || !self->PartitionNode)
        return PyUnicode_FromStringAndSize("", 1);

    std::stringstream outStream;
    int indention = 0;
    int outFilter = dumpConfig::dump_all;
 
    Py_BEGIN_ALLOW_THREADS;

    outStream << "{\n";
    indention++;


    writeModel(self->rootModel, outStream, indention, outFilter);

    RecurseDownTheTreeJson(dumpConfig::dump_all, indention, outStream, self->PartitionNode, "");

    indention--;
    outStream << '}' << endl;

    Py_END_ALLOW_THREADS;

    return PyUnicode_FromStringAndSize(outStream.str().c_str(), outStream.str().size());
}

static PyObject*
JtFile_writeGlTf(JtFileObject* self, PyObject* args , PyObject*)
{
    if (!self->rootModel || !self->PartitionNode)
        return PyUnicode_FromStringAndSize("", 1);

    char* value;
    if (!PyArg_ParseTuple(args, "s", &value)) {
        Py_INCREF(Py_False);
        return Py_False;
    }
   
 
    Py_BEGIN_ALLOW_THREADS;

    writeGltfMesh(self->PartitionNode, string(value));

    //RecurseDownTheTreeGlTf( self->PartitionNode, "");

    Py_END_ALLOW_THREADS;

    Py_INCREF(Py_True);
    return Py_True;
}


static PyMethodDef JtFile_methods[] = {
    { "open", (PyCFunction)JtFile_open, METH_VARARGS | METH_KEYWORDS ,"Open the file and read the LSG" },
    { "json", (PyCFunction)JtFile_json, METH_VARARGS | METH_KEYWORDS ,"Get the content of the Jt-File as JSON" },
    { "writeGlTf", (PyCFunction)JtFile_writeGlTf, METH_VARARGS | METH_KEYWORDS ,"Get the content of the Jt-File as JSON" },
    { NULL }  /* Sentinel */
};

static void
JtFile_dealloc(JtFileObject* self)
{
    std::destroy_at(&self->rootModel);
    std::destroy_at(&self->PartitionNode);
    std::destroy_at(&self->FileName);
    PyObject_Free(reinterpret_cast<PyObject*>(self));
}

static PyObject*
JtFile_new(PyTypeObject* type, PyObject*, PyObject*)
{
    auto* self = reinterpret_cast<JtFileObject*>(PyType_GenericAlloc(type, 0));
    if (self == nullptr)
        return nullptr;

    new (&self->FileName) std::string();
    new (&self->PartitionNode) std::shared_ptr<JtNode_Partition>();
    new (&self->rootModel) std::shared_ptr<JtData_Model>();
    return reinterpret_cast<PyObject*>(self);
}

static PyType_Slot JtFile_slots[] = {
    {Py_tp_dealloc, reinterpret_cast<void*>(JtFile_dealloc)},
    {Py_tp_doc, const_cast<char*>(JtFile_doc)},
    {Py_tp_methods, JtFile_methods},
    {Py_tp_getset, JtFile_getseters},
    {Py_tp_init, reinterpret_cast<void*>(JtFile_init)},
    {Py_tp_new, reinterpret_cast<void*>(JtFile_new)},
    {0, nullptr}
};

static PyType_Spec JtFile_spec = {
    "pyopenjt.JtFile",
    sizeof(JtFileObject),
    0,
    Py_TPFLAGS_DEFAULT,
    JtFile_slots
};

int addJtFileClass(PyObject* module)
{
    PyObject* jtFileType = PyType_FromSpec(&JtFile_spec);
    if (jtFileType == nullptr)
        return -1;

    if (PyModule_AddObject(module, "JtFile", jtFileType) < 0) {
        Py_DECREF(jtFileType);
        return -1;
    }

    return 0;
}

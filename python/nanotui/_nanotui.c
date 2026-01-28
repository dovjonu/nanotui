#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <nanotui/node.h>
#include <nanotui/layouts/vbox.h>
#include <nanotui/widgets/label.h>

/* ---------------- PyNode wrapper ---------------- */

typedef struct {
    PyObject_HEAD
    Node* node;
    int owns_node; /* 1 -> calls node_destroy in dealloc */
} PyNode;

/* Forward declaration of base type */
static PyTypeObject PyNodeType;

/* ---------------- Common helpers ---------------- */

static void PyNode_dealloc(PyNode* self) {
    if (self->owns_node && self->node) {
        node_destroy(self->node);
        self->node = NULL;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int require_valid_node(PyNode* n) {
    if (!n || !n->node) {
        PyErr_SetString(PyExc_RuntimeError, "Node is NULL");
        return 0;
    }
    return 1;
}

/* ---------------- Node.add(child) ---------------- */

static PyObject* PyNode_add(PyNode* self, PyObject* args) {
    PyObject* child_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &child_obj))
        return NULL;

    /* FIX: check against base Node type, not self type */
    if (!PyObject_TypeCheck(child_obj, &PyNodeType)) {
        PyErr_SetString(PyExc_TypeError, "child must be a nanotui Node");
        return NULL;
    }

    PyNode* child = (PyNode*)child_obj;

    if (!require_valid_node(self) || !require_valid_node(child))
        return NULL;

    node_add_child(self->node, child->node);

    /* Ownership transfer to the parent tree */
    child->owns_node = 0;

    Py_RETURN_NONE;
}

/* ---------------- Border helpers ---------------- */

static PyObject* PyNode_border_mask(PyNode* self, PyObject* args) {
    unsigned int mask = 0;
    if (!PyArg_ParseTuple(args, "I", &mask))
        return NULL;

    if (!require_valid_node(self))
        return NULL;

    node_border_set_mask(self->node, (BorderMask)mask);
    Py_RETURN_NONE;
}

static PyObject* PyNode_border_title(PyNode* self, PyObject* args) {
    const char* text = NULL;
    int pos = (int)BORDER_TITLE_LEFT;

    if (!PyArg_ParseTuple(args, "si", &text, &pos))
        return NULL;

    if (!require_valid_node(self))
        return NULL;

    BorderTitle t;
    t.text = text;
    t.position = (BorderTitlePosition)pos;
    node_border_set_title(self->node, t);

    Py_RETURN_NONE;
}

/* ---------------- Base Node type ---------------- */

static PyMethodDef node_methods[] = {
    {"add",          (PyCFunction)PyNode_add,          METH_VARARGS, "Add child node (transfers ownership)."},
    {"border_mask",  (PyCFunction)PyNode_border_mask,  METH_VARARGS, "Set border mask (int)."},
    {"border_title", (PyCFunction)PyNode_border_title, METH_VARARGS, "Set border title (text, position_int)."},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject PyNodeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nanotui._nanotui.Node",
    .tp_basicsize = sizeof(PyNode),
    .tp_dealloc = (destructor)PyNode_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = node_methods,
    .tp_doc = "Base nanotui Node",
};

/* ---------------- VBox type ---------------- */

static PyObject* PyVBox_new(PyTypeObject* type, PyObject* args, PyObject* kw) {
    static char* kwlist[] = {"spacing", NULL};
    int spacing = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "|i", kwlist, &spacing))
        return NULL;

    PyNode* self = (PyNode*)type->tp_alloc(type, 0);
    if (!self) return NULL;

    self->node = vbox_create(spacing);
    self->owns_node = 1;

    if (!self->node) {
        Py_DECREF(self);
        PyErr_SetString(PyExc_MemoryError, "vbox_create failed");
        return NULL;
    }

    return (PyObject*)self;
}

static PyTypeObject PyVBoxType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nanotui._nanotui.VBox",
    .tp_basicsize = sizeof(PyNode),
    .tp_base = &PyNodeType,                 /* FIX */
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyVBox_new,
};

/* ---------------- Label type ---------------- */

static PyObject* PyLabel_new(PyTypeObject* type, PyObject* args, PyObject* kw) {
    static char* kwlist[] = {"text", NULL};
    const char* text = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "s", kwlist, &text))
        return NULL;

    PyNode* self = (PyNode*)type->tp_alloc(type, 0);
    if (!self) return NULL;

    self->node = label_create(text);
    self->owns_node = 1;

    if (!self->node) {
        Py_DECREF(self);
        PyErr_SetString(PyExc_MemoryError, "label_create failed");
        return NULL;
    }

    return (PyObject*)self;
}

static PyTypeObject PyLabelType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nanotui._nanotui.Label",
    .tp_basicsize = sizeof(PyNode),
    .tp_base = &PyNodeType,                 /* FIX */
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyLabel_new,
};

/* ---------------- Module init ---------------- */

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_nanotui",
    "nanotui C extension",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit__nanotui(void) {
    if (PyType_Ready(&PyNodeType) < 0) return NULL;
    if (PyType_Ready(&PyVBoxType) < 0) return NULL;
    if (PyType_Ready(&PyLabelType) < 0) return NULL;

    PyObject* m = PyModule_Create(&moduledef);
    if (!m) return NULL;

    Py_INCREF(&PyNodeType);
    Py_INCREF(&PyVBoxType);
    Py_INCREF(&PyLabelType);

    PyModule_AddObject(m, "Node",  (PyObject*)&PyNodeType);
    PyModule_AddObject(m, "VBox",  (PyObject*)&PyVBoxType);
    PyModule_AddObject(m, "Label", (PyObject*)&PyLabelType);

    /* Export constants */
    PyModule_AddIntConstant(m, "BORDER_TOP", BORDER_TOP);
    PyModule_AddIntConstant(m, "BORDER_RIGHT", BORDER_RIGHT);
    PyModule_AddIntConstant(m, "BORDER_BOTTOM", BORDER_BOTTOM);
    PyModule_AddIntConstant(m, "BORDER_LEFT", BORDER_LEFT);

    PyModule_AddIntConstant(m, "BORDER_TITLE_LEFT", BORDER_TITLE_LEFT);
    PyModule_AddIntConstant(m, "BORDER_TITLE_CENTER", BORDER_TITLE_CENTER);
    PyModule_AddIntConstant(m, "BORDER_TITLE_RIGHT", BORDER_TITLE_RIGHT);

    return m;
}

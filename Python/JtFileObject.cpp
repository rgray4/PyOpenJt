
#include <Python.h>



const char* JtFile_doc = "Object representing an open JT file";
extern PyTypeObject JtFileClassType;
PyObject *JtFile_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

typedef struct {
    PyObject_HEAD
    void *world;
} JtFileObject;


/*
static PyObject *
JtFile_getDepth(JtFileObject *self, void *)
{
    self;
    if (self->world)
        return PyLong_FromLong(self->world->getDepth());

    return PyLong_FromLong(0);
}

static PyObject *
JtFile_getCount(JtFileObject *self, void *)
{
    if (!self->world)
        return PyLong_FromLongLong(0);

    CountVisitor counter;
    self->world->apply(counter);

    return PyLong_FromUnsignedLongLong(counter.count);
}
*/

static PyGetSetDef JtFile_getseters[] = {
 /*   { "depth",
    (getter)JtFile_getDepth, NULL , 
    "depth of the world",   NULL },
    { "count",
    (getter)JtFile_getCount, NULL , 
    "number of true bits in the world ",  NULL },*/
    { NULL }  /* Sentinel */
};


/*
static PyObject *
JtFile_advance(JtFileObject *self, PyObject *args, PyObject *)
{
    unsigned long num=0;
    // Parse the input tuple 
    if (!PyArg_ParseTuple(args, "k", num))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;

    self->world->advance(num);


    Py_END_ALLOW_THREADS;

    return Py_None;
}

static PyObject *
JtFile_getBit(JtFileObject *self, PyObject *args, PyObject *)
{
	long x = 0, y = 0, z = 0;
	// Parse the input tuple 
	if (!PyArg_ParseTuple(args, "lll", &x, &y, &z))
		return NULL;

	bool result = self->world->getBit(VectorInt3D64(x, y, z));

	if (result)
		return Py_True;
	else
		return Py_False;
}


static PyObject *
JtFile_getStatistic(PyObject *, PyObject *args)
{
    const char *type = nullptr;
    if (!PyArg_ParseTuple(args, "s", &type))
        return NULL;

    if (strcmp(type, "Pattern_Count"))
        return PyLong_FromLong(0);

    return Py_None;
}
*/

static PyMethodDef JtFile_methods[] = {
 /*   { "advance", (PyCFunction)JtFile_advance, METH_VARARGS | METH_KEYWORDS ,
    "Return the name, combining the first and last name"
    },
	{ "getBit", (PyCFunction)JtFile_getBit, METH_VARARGS | METH_KEYWORDS ,
	"Returns the bit with the given position"
	},
	{ "getStatistic", (PyCFunction)JtFile_getStatistic, METH_VARARGS | METH_KEYWORDS ,
    "Returns statistic on the world. Types can be:\n"
    "'Bit_Count'    : returns number of true bits in the world\n"
    },*/
    { NULL }  /* Sentinel */
};

static void
JtFile_dealloc(JtFileObject* self)
{
    //delete self->world;
	//self->world = NULL;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int
JtFile_init(JtFileObject *self, PyObject *args, PyObject *kwds)
{
    kwds; args;
    /*PyObject *first = NULL, *last = NULL;
    int level = 1;
    int seed1 = 0;
    int seed2 = 0;

    static const char *kwlist[] = { "level", "seed1", "seed2", NULL };
	PyObject *object = nullptr;

	if (PyArg_ParseTupleAndKeywords(args, kwds, "i|ii", const_cast<char**>(kwlist), &level, &seed1, &seed2)) {

		if (seed1 != 0 || seed2 != 0)
			self->world = World::buildRandomWorld(level, seed1, seed2);
		else
			self->world = World::buildEmptyWorld(level);
		return 0;
	}*/
	//else if (PyArg_ParseTuple(args, "O", &object)) {
	//	if (PySequence_Check(object)) {
	//		auto length = PySequence_Length(object);
	//		if (length != 8) return -1;
	//		std::vector<uint64_t> startSet;
	//		startSet.reserve(length);
	//		for (int i = 0; i <= length; i++) {
	//			PyObject *elem = PySequence_GetItem(object, i);
	//			if (!PyLong_Check(elem)) return -1;
	//			uint64_t e = PyLong_AsUnsignedLongLong(elem);
	//			startSet.push_back(e);
	//		}

	//		self->world = World::buildWorldFromArray(startSet);


	//	}
	//}

    //self->world = new World();

    return 0;
}

PyObject *
JtFile_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    kwds; args;
    JtFileObject *self;

    self = (JtFileObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
       self->world = nullptr; 
    }

    return (PyObject *)self;
}

PyTypeObject JtFileClassType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bcapy.World",      /* tp_name */
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
	PyModule_AddObject(module, "World", (PyObject *)&JtFileClassType);
}



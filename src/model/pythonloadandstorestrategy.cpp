#include "pythonloadandstorestrategy.hpp"
#include "pythoncontext.hpp"

#include <Python.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

PythonLoadAndStoreStrategy::PythonLoadAndStoreStrategy() {
}

PythonLoadAndStoreStrategy::~PythonLoadAndStoreStrategy() {
}

bool PythonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {
    return true;
}

QList<ImagePtr> PythonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;

    // Calls PyInitialize() on creation and PyFinalize() when it is destroyed
    PythonContext context;

    //PyRun_SimpleString("import sys; sys.path.append('.');");

    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pReturnValue, *pValueImagesPath, *pValueSegmentationImagesPath;

    pName = PyUnicode_FromString(testScript.toUtf8().data());

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "load_images");

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(2);
            pValueImagesPath = PyUnicode_FromString(imagesPath.toUtf8().data());
            if (!segmentationImagesPath.isNull()) {
                pValueSegmentationImagesPath = PyUnicode_FromString(segmentationImagesPath.toUtf8().data());
            } else {
                pValueSegmentationImagesPath = Py_None;
            }
            PyTuple_SetItem(pArgs, 0, pValueImagesPath);
            PyTuple_SetItem(pArgs, 1, pValueSegmentationImagesPath);
            pReturnValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pReturnValue != NULL) {
                if (PyList_Check(pReturnValue)) {
                    int images_size = PyList_Size(pReturnValue);
                    for (int i = 0; i < images_size; i++) {
                        PyObject *dict = PyList_GetItem(pReturnValue, i);
                        if (PyDict_Check(dict)) {
                            PyObject *key = PyUnicode_FromString("filename");
                            PyObject *filename = PyDict_GetItemWithError(dict, key);
                            if (filename != NULL) {

                            } else {
                                PyErr_Print();
                                fprintf(stderr,"Failed to get entry from dict.\n");
                            }
                        } else {
                            // Not a dict
                        }
                    }
                } else {
                    // Not a list
                }
            }
            else {
                PyErr_Print();
                fprintf(stderr,"Call failed.\n");
            }
            Py_DECREF(pReturnValue);
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            qDebug() << "Cannot find function load_images.";
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        qDebug() << "Error importing module.";
    }
    return images;
}

QList<QString> PythonLoadAndStoreStrategy::imagesWithInvalidCameraMatrix() const {
    return QList<QString>();
}

QList<PosePtr> PythonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images, const QList<ObjectModelPtr> &objectModels) {
    return QList<PosePtr>();
}

QList<QString> PythonLoadAndStoreStrategy::posesWithInvalidPosesData() const {
    return QList<QString>();
}

#include "pythonloadandstorestrategy.hpp"

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <QFileInfo>
#include <QList>

namespace py = pybind11;

PythonLoadAndStoreStrategy::PythonLoadAndStoreStrategy() {
    py::initialize_interpreter();
}

PythonLoadAndStoreStrategy::~PythonLoadAndStoreStrategy() {
    keepRunning = false;
    py::finalize_interpreter();
}

QList<ImagePtr> PythonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;

    QFileInfo fileInfo(loadSaveScript);
    if (!fileInfo.exists()) {
        // TODO error
        return images;
    }

    QString dirname = fileInfo.dir().absolutePath();
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


    QString filename = fileInfo.fileName();
    py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

    py::object result = script.attr("load_images")(imagesPath.toUtf8().data(), py::none());

    if (py::isinstance<py::list>(result)) {
        py::list result_list = py::list(result);
        int length = result_list.size();
        for (int i = 0; i < length && keepRunning; i++) {
            py::object item = result_list[i];
            if (py::isinstance<py::dict>(item)) {
                py::dict item_dict = py::dict(item);
                QString imagePath, basePath, segmentationImagePath;
                float nearPlane, farPlane;
                QMatrix3x3 cameraMatrix;
                if (item_dict.contains("image_path")) {
                    py::object imagePathItem = item_dict["image_path"];
                    if (py::isinstance<py::str>(imagePathItem)) {
                        imagePath = QString::fromStdString(
                                    item_dict["image_path"].cast<std::string>());
                    } else {
                        qDebug() << "error 1";
                        continue;
                        // image_path not a string -> error handling
                        // print & add to invalid data &  continue
                    }
                } else {
                    qDebug() << "error 2";
                    continue;
                    // Doesn't contain image_path -> error handling
                    // print & add to invalid data &  continue
                }
                if (item_dict.contains("base_path")) {
                    py::object basePathItem = item_dict["base_path"];
                    if (py::isinstance<py::str>(basePathItem)) {
                        basePath = QString::fromStdString(
                                    item_dict["base_path"].cast<std::string>());
                    } else {
                        qDebug() << "error 3";
                        continue;
                        // base_path not a string -> error handling
                        // print & add to invalid data &  continue
                    }
                } else {
                    qDebug() << "error 4";
                    continue;
                    // Doesn't contain base_path -> error handling
                    // print & add to invalid data &  continue
                }
                if (item_dict.contains("segmentation_image_path")) {
                    py::object segmentationImagePathItem = item_dict["segmentation_image_path"];
                    if (py::isinstance<py::str>(segmentationImagePathItem)) {
                        segmentationImagePath = QString::fromStdString(
                                    item_dict["segmentation_image_path"].cast<std::string>());
                    } else {
                        qDebug() << "error 5";
                        continue;
                        // segmentation_image_path not a string -> error handling
                        // print & add to invalid data &  continue
                    }
                } else {
                    // Doesn't contain segmentation_image_path -> NO error handling, it's not required
                }
                if (item_dict.contains("K")) {
                    py::object KItem = item_dict["K"];
                    if (py::isinstance<py::list>(KItem)) {
                        py::list K = py::list(KItem);
                        int KSize = K.size();
                        if (KSize == 9) {
                            float KConverted[9];
                            for (int j = 0; j < KSize; j++) {
                                if (py::isinstance<py::float_>(K[j])) {
                                    KConverted[j] = K[j].cast<float>();
                                } else {
                                    qDebug() << "error 7";
                                    continue;
                                    // K contains non-float values
                                }
                            }
                            cameraMatrix = QMatrix3x3(KConverted);
                        } else {
                            qDebug() << "error 8";
                            continue;
                            // K is not a 3x3 matrix
                        }
                    } else {
                        qDebug() << "error 9";
                        continue;
                        // K is not a list of floats -> error handling
                        // print & add to invalid data &  continue
                    }
                } else {
                    qDebug() << "error 10";
                    continue;
                    // Doesn't contain camera matrix -> error handling
                    // print & add to invalid data &  continue
                }
                if (item_dict.contains("near_plane")) {
                    //py::object nearPlaneItem = item_dict["near_plane"];
                    nearPlane = 50;
                } else {
                    nearPlane = 50;
                }
                if (item_dict.contains("far_plane")) {
                    farPlane = 2000;
                } else {
                    farPlane = 2000;
                }
                images.append(ImagePtr(new Image(imagePath, basePath, cameraMatrix, nearPlane, farPlane)));
            } else {
                qDebug() << "error 11";
                // Not a dict
            }
        }
    } else {
        qDebug() << "error 12";
        // Not a list
    }

    return images;
}

QList<QString> PythonLoadAndStoreStrategy::imagesWithInvalidData() const {
    return QList<QString>();
}

bool PythonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {
    return true;
}

QList<PosePtr> PythonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images,
                                                     const QList<ObjectModelPtr> &objectModels) {
    QList<PosePtr> poses;

    QFileInfo fileInfo(loadSaveScript);
    if (!fileInfo.exists()) {
        // TODO error
        return poses;
    }

    // TODO remove

    QString dirname = fileInfo.dir().absolutePath();
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


    QString filename = fileInfo.fileName();
    py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

    /*
    py::object result = script.attr("load_poses")(imagesPath.toUtf8().data(), py::none());
    py::print(result);
    */

    return poses;
}

QList<QString> PythonLoadAndStoreStrategy::posesWithInvalidData() const {
    return QList<QString>();
}

void PythonLoadAndStoreStrategy::setLoadSaveScript(const QString &value) {
    loadSaveScript = value;
}

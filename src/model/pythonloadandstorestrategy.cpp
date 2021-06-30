#include "pythonloadandstorestrategy.hpp"
#include "generalhelper.hpp"

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <QFileInfo>
#include <QList>
#include <QElapsedTimer>

namespace py = pybind11;

PythonLoadAndStoreStrategy::PythonLoadAndStoreStrategy() {
    py::initialize_interpreter();
}

PythonLoadAndStoreStrategy::~PythonLoadAndStoreStrategy() {
    py::finalize_interpreter();
}

void PythonLoadAndStoreStrategy::applySettings(SettingsPtr settings) {
    m_loadSaveScript = settings->loadSaveScriptPath();
    LoadAndStoreStrategy::applySettings(settings);
}

QList<ImagePtr> PythonLoadAndStoreStrategy::loadImages() {
    QElapsedTimer timer;
    timer.start();
    QList<ImagePtr> images;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        // TODO error
        return images;
    }

    try {
        QString dirname = fileInfo.dir().absolutePath();
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


        QString filename = fileInfo.fileName();
        py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

        py::object result = script.attr("load_images")(m_imagesPath.toUtf8().data(), py::none());

        if (py::isinstance<py::list>(result)) {
            py::list result_list = py::list(result);
            int length = result_list.size();
            for (int i = 0; i < length; i++) {
                py::object item = result_list[i];
                if (py::isinstance<py::dict>(item)) {
                    py::dict itemDict = py::dict(item);
                    QString imagePath, basePath, segmentationImagePath;
                    float nearPlane, farPlane;
                    QMatrix3x3 cameraMatrix;
                    if (itemDict.contains("image_path")) {
                        py::object imagePathItem = itemDict["image_path"];
                        if (py::isinstance<py::str>(imagePathItem)) {
                            imagePath = QString::fromStdString(
                                        itemDict["image_path"].cast<std::string>());
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
                    if (itemDict.contains("base_path")) {
                        py::object basePathItem = itemDict["base_path"];
                        if (py::isinstance<py::str>(basePathItem)) {
                            basePath = QString::fromStdString(
                                        itemDict["base_path"].cast<std::string>());
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
                    if (itemDict.contains("segmentation_image_path")) {
                        py::object segmentationImagePathItem = itemDict["segmentation_image_path"];
                        if (py::isinstance<py::str>(segmentationImagePathItem)) {
                            segmentationImagePath = QString::fromStdString(
                                        itemDict["segmentation_image_path"].cast<std::string>());
                        } else {
                            qDebug() << "error 5";
                            continue;
                            // segmentation_image_path not a string -> error handling
                            // print & add to invalid data &  continue
                        }
                    } else {
                        // Doesn't contain segmentation_image_path -> NO error handling, it's not required
                    }
                    if (itemDict.contains("K")) {
                        py::object KItem = itemDict["K"];
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
                    if (itemDict.contains("near_plane")) {
                        //py::object nearPlaneItem = item_dict["near_plane"];
                        nearPlane = 50;
                    } else {
                        nearPlane = 50;
                    }
                    if (itemDict.contains("far_plane")) {
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
        } else if (py::isinstance<py::str>(result)) {
            // The script returned an error -> pass it on to the view
            qDebug() << "error 14";
        } else {
            qDebug() << "error 12";
            // Not a list
        }
    } catch (py::error_already_set &e) {
        // Some crazy error happened
        qDebug() << "error 13";
    }
    qDebug() << "Loading images took " << timer.elapsed() << " miliseconds";
    return images;
}

bool PythonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {
    return true;
}

QList<PosePtr> PythonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images,
                                                     const QList<ObjectModelPtr> &objectModels) {
    QElapsedTimer timer;
    timer.start();
    QList<PosePtr> poses;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        // TODO error
        return poses;
    }

    try {
        QString dirname = fileInfo.dir().absolutePath();
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


        QString filename = fileInfo.fileName();
        py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

        py::object result = script.attr("load_poses")(m_posesFilePath.toUtf8().data());

        if (py::isinstance<py::list>(result)) {
            py::list result_list = py::list(result);
            int length = result_list.size();
            for (int i = 0; i < length; i++) {
                py::object itemPosesForImages = result_list[i];
                if (py::isinstance<py::list>(itemPosesForImages)) {
                    py::list listPosesForImages = py::list(itemPosesForImages);
                    int length = listPosesForImages.size();
                    for (int j = 0; j < length; j++) {
                        if (py::isinstance<py::dict>(listPosesForImages[j])) {
                            py::dict poseDict = py::dict(listPosesForImages[j]);
                            QString poseID;
                            int imgID, objID;
                            QMatrix3x3 rotation;
                            QVector3D translation;
                            if (poseDict.contains("img_id")) {
                                py::object imgIDItem = poseDict["img_id"];
                                py::isinstance<py::int_>(imgIDItem);
                                if (py::isinstance<py::int_>(imgIDItem)) {
                                    imgID = poseDict["img_id"].cast<int>();
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
                            if (poseDict.contains("obj_id")) {
                                py::object objIDItem = poseDict["obj_id"];
                                if (py::isinstance<py::int_>(objIDItem)) {
                                    objID = poseDict["obj_id"].cast<int>();
                                } else {
                                    qDebug() << "error 3";
                                    continue;
                                    // image_path not a string -> error handling
                                    // print & add to invalid data &  continue
                                }
                            } else {
                                qDebug() << "error 4";
                                continue;
                                // Doesn't contain image_path -> error handling
                                // print & add to invalid data &  continue
                            }
                            // Get rotation
                            if (poseDict.contains("R")) {
                                py::object RItem = poseDict["R"];
                                if (py::isinstance<py::list>(RItem)) {
                                    py::list R = py::list(RItem);
                                    int RSize = R.size();
                                    if (RSize == 9) {
                                        float RConverted[9];
                                        for (int j = 0; j < RSize; j++) {
                                            if (py::isinstance<py::float_>(R[j])) {
                                                RConverted[j] = R[j].cast<float>();
                                            } else {
                                                qDebug() << "error 5";
                                                continue;
                                                // K contains non-float values
                                            }
                                        }
                                        rotation = QMatrix3x3(RConverted);
                                    } else {
                                        qDebug() << "error 6";
                                        continue;
                                        // K is not a 3x3 matrix
                                    }
                                } else {
                                    qDebug() << "error 7";
                                    continue;
                                    // K is not a list of floats -> error handling
                                    // print & add to invalid data &  continue
                                }
                            } else {
                                qDebug() << "error 8";
                                continue;
                                // Doesn't contain camera matrix -> error handling
                                // print & add to invalid data &  continue
                            }
                            // Get translation
                            if (poseDict.contains("t")) {
                                py::object tItem = poseDict["t"];
                                if (py::isinstance<py::list>(tItem)) {
                                    py::list t = py::list(tItem);
                                    int tSize = t.size();
                                    if (tSize == 3) {
                                        float tConverted[3];
                                        for (int j = 0; j < tSize; j++) {
                                            if (py::isinstance<py::float_>(t[j])) {
                                                tConverted[j] = t[j].cast<float>();
                                            } else {
                                                qDebug() << "error 9";
                                                continue;
                                                // K contains non-float values
                                            }
                                        }
                                        translation = QVector3D(tConverted[0],
                                                                tConverted[1],
                                                                tConverted[2]);
                                    } else {
                                        qDebug() << "error 10";
                                        continue;
                                        // K is not a 3x3 matrix
                                    }
                                } else {
                                    qDebug() << "error 11";
                                    continue;
                                    // K is not a list of floats -> error handling
                                    // print & add to invalid data &  continue
                                }
                            } else {
                                qDebug() << "error 12";
                                continue;
                                // Doesn't contain camera matrix -> error handling
                                // print & add to invalid data &  continue
                            }

                            if (imgID < 0 || imgID >= images.size() || objID < 0 || objID >= objectModels.size()) {
                                // Error
                                qDebug() << "Error 16" << imgID << objID;
                                continue;
                            }

                            ImagePtr image = images[imgID];
                            ObjectModelPtr objectModel = objectModels[objID];

                            if (poseDict.contains("pose_id")) {
                                py::object poseIDItem = poseDict["pose_id"];
                                if (py::isinstance<py::str>(poseIDItem)) {
                                    poseID = QString::fromStdString(poseDict["pose_id"].cast<std::string>());
                                } else {
                                    qDebug() << "error 1";
                                    continue;
                                    // image_path not a string -> error handling
                                    // print & add to invalid data &  continue
                                }
                            } else {
                                // No error, if no ID is present, create one
                                poseID = GeneralHelper::createPoseId(*image, *objectModel);
                            }
                            poses.append(PosePtr(new Pose(poseID, translation, rotation, image, objectModel)));
                        } else {
                            qDebug() << "Error 5000";
                        }
                    }
                } else {
                    py::print(itemPosesForImages);
                    // Not a list of entries for image
                    qDebug() << "poses error 13";
                }
            }
        } else if (py::isinstance<py::str>(result)) {
            // The script returned an error -> pass it on to the view
            qDebug() << "error 14";
        } else {
            qDebug() << "error 15";
            // Not a list
        }
    }  catch (py::error_already_set &e) {
        qDebug() << "poses error 16";
    }
    qDebug() << "Loading poses took " << timer.elapsed() << " miliseconds";

    return poses;
}

#include "pythonloadandstorestrategy.hpp"
#include "generalhelper.hpp"

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
    py::finalize_interpreter();
}

void PythonLoadAndStoreStrategy::applySettings(SettingsPtr settings) {
    m_loadSaveScript = settings->loadSaveScriptPath();
    LoadAndStoreStrategy::applySettings(settings);
}

bool PythonLoadAndStoreStrategy::extractPath() {

}

bool PythonLoadAndStoreStrategy::extractID() {

}

bool PythonLoadAndStoreStrategy::extractVector() {

}

bool PythonLoadAndStoreStrategy::extractFloat() {

}

QList<ImagePtr> PythonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        qDebug() << "script not found";
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
                    QString imageID, imagePath, basePath, segmentationImagePath;
                    float nearPlane, farPlane;
                    QMatrix3x3 cameraMatrix;
                    if (itemDict.contains("img_id")) {
                        py::object imageIDItem = itemDict["img_id"];
                        // image_id can be a string or an int
                        if (py::isinstance<py::str>(imageIDItem)) {
                            imageID = QString::fromStdString(
                                        itemDict["img_id"].cast<std::string>());
                        } else if (py::isinstance<py::int_>(imageIDItem)) {
                            imageID = QString::number(
                                        itemDict["img_id"].cast<int>());
                        } else {
                            qDebug() << "error 1";
                            continue;
                            // image_id not a string -> error handling
                            // print & add to invalid data &  continue
                        }
                    } else {
                        // If we don't find any ID simply use the index
                        imageID = QString::number(i);
                    }
                    if (itemDict.contains("img_path")) {
                        py::object imagePathItem = itemDict["img_path"];
                        if (py::isinstance<py::str>(imagePathItem)) {
                            imagePath = QString::fromStdString(
                                        itemDict["img_path"].cast<std::string>());
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
                    images.append(ImagePtr(new Image(imageID, imagePath, basePath,
                                                     cameraMatrix, nearPlane, farPlane)));
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
        qDebug() << e.what();
        // Some crazy error happened
        qDebug() << "image error 13";
    }
    return images;
}

QList<ObjectModelPtr> PythonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModelPtr> objectModels;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        qDebug() << "script not found";
        return objectModels;
    }

    try {
        QString dirname = fileInfo.dir().absolutePath();
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


        QString filename = fileInfo.fileName();
        py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

        py::object result = script.attr("load_object_models")(m_objectModelsPath.toUtf8().data());

        if (py::isinstance<py::list>(result)) {
            py::list result_list = py::list(result);
            int length = result_list.size();
            for (int i = 0; i < length; i++) {
                py::object item = result_list[i];
                if (py::isinstance<py::dict>(item)) {
                    py::dict itemDict = py::dict(item);
                    QString objID, objectModelPath, basePath;
                    if (itemDict.contains("obj_id")) {
                        py::object objIDItem = itemDict["obj_id"];
                        // image_id can be a string or an int
                        if (py::isinstance<py::str>(objIDItem)) {
                            objID = QString::fromStdString(
                                        itemDict["obj_id"].cast<std::string>());
                        } else if (py::isinstance<py::int_>(objIDItem)) {
                            objID = QString::number(
                                        itemDict["obj_id"].cast<int>());
                        } else {
                            qDebug() << "error 1";
                            continue;
                            // image_id not a string -> error handling
                            // print & add to invalid data &  continue
                        }
                    } else {
                        // If we don't find any ID simply use the index
                        objID = QString::number(i);
                    }
                    if (itemDict.contains("obj_model_path")) {
                        py::object objModelPathItem = itemDict["obj_model_path"];
                        // image_id can be a string or an int
                        if (py::isinstance<py::str>(objModelPathItem)) {
                            objectModelPath = QString::fromStdString(
                                        itemDict["obj_model_path"].cast<std::string>());
                        } else {
                            qDebug() << "error 1";
                            continue;
                            // image_id not a string -> error handling
                            // print & add to invalid data &  continue
                        }
                    } else {
                        // If we don't find any ID simply use the index
                        qDebug() << "Error";
                    }
                    if (itemDict.contains("base_path")) {
                        py::object basePathItem = itemDict["base_path"];
                        // image_id can be a string or an int
                        if (py::isinstance<py::str>(basePathItem)) {
                            basePath = QString::fromStdString(
                                        itemDict["base_path"].cast<std::string>());
                        } else {
                            qDebug() << "error 1";
                            continue;
                            // image_id not a string -> error handling
                            // print & add to invalid data &  continue
                        }
                    } else {
                        // If we don't find any ID simply use the index
                        qDebug() << "Error";
                    }
                    objectModels.append(ObjectModelPtr(new ObjectModel(objID, objectModelPath, basePath)));
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
        qDebug() << e.what();
        // Some crazy error happened
        qDebug() << "obj error 13";
    }
    return objectModels;
}

bool PythonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {
    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        qDebug() << "Script not found";
        return false;
    }

    try {
        QString dirname = fileInfo.dir().absolutePath();
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, dirname.toUtf8().data());


        QString filename = fileInfo.fileName();
        py::module script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());

        QString poseIdD = objectImagePose.id();
        QString imageID = objectImagePose.image()->id();
        QString objID = objectImagePose.objectModel()->id();
        QString imagePath = objectImagePose.image()->imagePath();
        QString objectModelPath = objectImagePose.objectModel()->path();
        const float *rotation = objectImagePose.rotation().toRotationMatrix().constData();
        const float translation[3] = {objectImagePose.position()[0],
                                      objectImagePose.position()[1],
                                      objectImagePose.position()[2]};

        py::object result = script.attr("save_pose")(m_posesFilePath,
                                                     poseIdD,
                                                     imageID,
                                                     imagePath,
                                                     objID,
                                                     objectModelPath,
                                                     rotation,
                                                     translation,
                                                     deletePose);
    } catch (py::error_already_set &e) {
        qDebug() << e.what();
        qDebug() << "obj error 13";
    }

    return true;
}

QList<PosePtr> PythonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images,
                                                     const QList<ObjectModelPtr> &objectModels) {
    QList<PosePtr> poses;
    // For faster lookup by ID
    QMap<QString, ImagePtr> imagesForID;
    QMap<QString, ImagePtr> imagesForPath;
    Q_FOREACH(ImagePtr image, images) {
        if (imagesForID.contains(image->id())) {
            qDebug() << "whoops two images with the same ID Error shouldn't happen";
        }
        imagesForID[image->id()] = image;
        imagesForPath[image->imagePath()] = image;
    }
    QMap<QString, ObjectModelPtr> objectModelsForID;
    QMap<QString, ObjectModelPtr> objectModelsForPath;
    Q_FOREACH(ObjectModelPtr objectModel, objectModels) {
        if (objectModelsForID.contains(objectModel->id())) {
            qDebug() << "whoops two object models with the same ID Error shouldn't happen";
        }
        objectModelsForID[objectModel->id()] = objectModel;
        objectModelsForPath[objectModel->path()] = objectModel;
    }

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        qDebug() << "Script not found";
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
                            ImagePtr image;
                            ObjectModelPtr objectModel;
                            QMatrix3x3 rotation;
                            QVector3D translation;
                            if (poseDict.contains("img_id")) {
                                py::object imgIDItem = poseDict["img_id"];
                                py::isinstance<py::int_>(imgIDItem);
                                QString imageID;
                                if (py::isinstance<py::int_>(imgIDItem)) {
                                    imageID = QString::number(imgIDItem.cast<int>());
                                } else if (py::isinstance<py::str>(imgIDItem)) {
                                    imageID = QString::fromStdString(imgIDItem.cast<std::string>());
                                } else {
                                    qDebug() << "error 1";
                                    continue;
                                    // image_path not a string -> error handling
                                    // print & add to invalid data &  continue
                                }
                                if (imagesForID.contains(imageID)) {
                                    image = imagesForID[imageID];
                                } else {
                                    qDebug() << "Image not found Error";
                                    continue;
                                }
                            } else if (poseDict.contains("img_path")) {
                                py::object imagePathItem = poseDict["img_path"];
                                if (py::isinstance<py::str>(imagePathItem)) {
                                    QString imagePath = QString::fromStdString(imagePathItem.cast<std::string>());
                                    if (imagesForPath.contains(imagePath)) {
                                        image = imagesForPath[imagePath];
                                    } else {
                                        qDebug() << "image_path found but no corresponding image exists";
                                        continue;
                                    }
                                } else {
                                    qDebug() << "image_path found but is not a string";
                                    continue;
                                }
                            } else {
                                qDebug() << "error 2";
                                continue;
                                // Doesn't contain image_path -> error handling
                                // print & add to invalid data &  continue
                            }
                            if (poseDict.contains("obj_id")) {
                                py::object objIDItem = poseDict["obj_id"];
                                QString objectModelID;
                                if (py::isinstance<py::int_>(objIDItem)) {
                                    objectModelID = QString::number(objIDItem.cast<int>());
                                } else if (py::isinstance<py::str>(objIDItem)) {
                                    objectModelID = QString::fromStdString(objIDItem.cast<std::string>());
                                } else {
                                    qDebug() << "error 3";
                                    continue;
                                    // image_path not a string -> error handling
                                    // print & add to invalid data &  continue
                                }
                                if (objectModelsForID.contains(objectModelID)) {
                                    objectModel = objectModelsForID[objectModelID];
                                } else {
                                    qDebug() << "Image not found Error";
                                    continue;
                                }
                            } else if (poseDict.contains("obj_model_path")) {
                                py::object objectModelPathItem = poseDict["obj_model_path"];
                                if (py::isinstance<py::str>(objectModelPathItem)) {
                                    QString objectModelPath = QString::fromStdString(objectModelPathItem.cast<std::string>());
                                    if (objectModelsForPath.contains(objectModelPath)) {
                                        objectModel = objectModelsForPath[objectModelPath];
                                    } else {
                                        qDebug() << "object_model_path found but no corresponding object model exists";
                                        continue;
                                    }
                                } else {
                                    qDebug() << "object_model_path found but is not a string";
                                    continue;
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

                            if (image.isNull() || objectModel.isNull()) {
                                qDebug() << "weird error";
                                continue;
                            }

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
                                // TODO Error: Our code works based on IDs and requires them
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
        qDebug() << e.what();
        qDebug() << "poses error 16";
    }

    return poses;
}

#include "pythonloadandstorestrategy.hpp"
#include "misc/generalhelper.hpp"
#include "misc/global.hpp"

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <QFileInfo>
#include <QList>
#include <QElapsedTimer>

PythonLoadAndStoreStrategy::PythonLoadAndStoreStrategy() {
    py::initialize_interpreter();
    sys  = py::module::import("sys");
}

PythonLoadAndStoreStrategy::~PythonLoadAndStoreStrategy() {
    // Necessary to avoid crashes
    script.dec_ref();
    py::finalize_interpreter();
}

void PythonLoadAndStoreStrategy::applySettings(SettingsPtr settings) {
    m_loadSaveScript = settings->loadSaveScriptPath();
    QFileInfo fileInfo(m_loadSaveScript);
    if (scriptInitialized) {
        // Remove inserted path
        sys.attr("path").attr("remove")(0);
    }
    QString dirname = fileInfo.dir().absolutePath();
    sys.attr("path").attr("insert")(0, dirname.toUtf8().data());
    QString filename = fileInfo.fileName();
    if (scriptInitialized) {
        script.reload();
    } else {
        script = py::module::import(filename.mid(0, filename.length() - 3).toUtf8().data());
    }
    scriptInitialized = true;
    LoadAndStoreStrategy::applySettings(settings);
}

bool PythonLoadAndStoreStrategy::extractPath(py::dict &dict, const char *key,
                                             QString &toSet, const QString &type,
                                             const QString &identifier,
                                             QList<QString> &invalidData,
                                             bool required) {
    if (dict.contains(key)) {
        py::object pathItem = dict[key];
        if (py::isinstance<py::str>(pathItem)) {
            toSet = QString::fromStdString(
                        dict[key].cast<std::string>());
            return true;
        } else {
            qDebug() << "\'" + QString::fromStdString(key) + "\'" + " in return dict but is"
                        " not a string (" + identifier + "). Skipping " + type + ".";
            invalidData.append(identifier);
            return false;
        }
    } else if (!required) {
        return true;
    } else {
        qDebug() << "\'" + QString::fromStdString(key) + "\'" + " not in return dict but is "
                    "required (ID: " + identifier + "). Skipping image.";
        m_imagesWithInvalidData.append(identifier);
        return false;
    }
}

bool PythonLoadAndStoreStrategy::extractID(py::dict &dict, const char *key,
                                           QString &toSet, const QString &type,
                                           const int defaultID) {
    if (dict.contains(key)) {
        py::object imageIDItem = dict[key];
        // image_id can be a string or an int
        if (py::isinstance<py::str>(imageIDItem)) {
            toSet = QString::fromStdString(dict[key].cast<std::string>());
        } else if (py::isinstance<py::int_>(imageIDItem)) {
            toSet = QString::number(dict[key].cast<int>());
        } else {
            qDebug() << "\'" + QString::fromStdString(key) + "\'" + " in return dict but is neither "
                        "string nor int (Index: " + QString::number(defaultID) + ")."
                        " Skipping " + type + ".";
            m_imagesWithInvalidData.append(QString::number(defaultID));
            return false;
        }
    } else {
        // If we don't find any ID simply use the index
        toSet = QString::number(defaultID);
    }
    return true;
}



template<class T>
bool PythonLoadAndStoreStrategy::extractIDOrPathAndRetrieve(pybind11::dict &dict, const char *id_key,
                                                            const char *path_key, int i, int j,
                                                            T &itemToSet, QList<QString> &invalidData,
                                                            QMap<QString, T> itemsForID,
                                                            QMap<QString, T> itemsForPath,
                                                            const QString &type) {
    if (dict.contains(id_key)) {
        py::object idItem = dict[id_key];
        py::isinstance<py::int_>(idItem);
        QString id;
        if (py::isinstance<py::int_>(idItem)) {
            id = QString::number(idItem.cast<int>());
        } else if (py::isinstance<py::str>(idItem)) {
            id = QString::fromStdString(idItem.cast<std::string>());
        } else {
            qDebug() << "\"img_id\" in return dict but is neither "
                        "string nor int (Index: "  << i << ", " << j << "). Skipping " + type + ".";
            invalidData.append(QString::number(i) + ", " +
                                          QString::number(j));
            return false;
        }
        if (itemsForID.contains(id)) {
            itemToSet = itemsForID[id];
            return true;
        } else {
            qDebug() << "The specified image " << id << " "
                        "could not be found. Skipping " + type + ".";
            invalidData.append(QString::number(i) + ", " +
                                          QString::number(j));
            return false;
        }
    } else if (dict.contains(path_key)) {
        py::object pathItem = dict[path_key];
        if (py::isinstance<py::str>(pathItem)) {
            QString path = QString::fromStdString(pathItem.cast<std::string>());
            if (itemsForPath.contains(path)) {
                itemToSet = itemsForPath[path];
                return true;
            } else {
                qDebug() << "The specified image " << path << " "
                            "could not be found. Skipping " + type + ".";
                invalidData.append(QString::number(i) + ", " +
                                              QString::number(j));
                return false;
            }
        } else {
            qDebug() << "\"img_path\" in return dict but is neither "
                        "string nor int (Index: "  << i << ", " << j << "). Skipping " + type + ".";
            invalidData.append(QString::number(i) + ", " +
                                          QString::number(j));
            return false;
        }
    } else {
        qDebug() << "The return dict for the pose contains neither "
                    "\"img_id\" nor \"img_path\" but one needs to be present "
                    "(Index: "  << i << ", " << j <<  "). Skipping " + type + ".";
        invalidData.append(QString::number(i) + ", " +
                                      QString::number(j));
        return false;
    }
}

bool PythonLoadAndStoreStrategy::extract3x3Matrix(py::dict &dict, const char *key,
                                                  QMatrix3x3 &toSet, const QString &parentType,
                                                  const QString &name,
                                                  const QString &identifier,
                                                  QList<QString> &invalidData) {
    if (dict.contains(key)) {
        py::object item = dict[key];
        if (py::isinstance<py::list>(item)) {
            py::list list = py::list(item);
            int size = list.size();
            if (size == 9) {
                float converted[9];
                for (int j = 0; j < size; j++) {
                    if (py::isinstance<py::float_>(list[j])) {
                        converted[j] = list[j].cast<float>();
                    } else {
                        qDebug() << name + "contains non-float values "
                                    "(" + identifier + "). Skipping " + parentType + ".";
                        invalidData.append(identifier);
                        return false;
                    }
                }
                toSet = QMatrix3x3(converted);
                return true;
            } else {
                qDebug() << name + " is not a 3x3 matrix "
                            "(doesn't contain 9 entries)  (" + identifier + "). "
                            "Skipping " + parentType + ".";
                invalidData.append(identifier);
                return false;
            }
        } else {
            qDebug() << name + " is not a list of floats "
                        "(" + identifier + "). Skipping " + parentType + ".";
            invalidData.append(identifier);
            return false;
        }
    } else {
        qDebug() << name + " \"" + key + "\" not in return dict "
                    "(" + identifier + "). Skipping " + parentType + ".";
        invalidData.append(identifier);
        return false;
    }
}

bool PythonLoadAndStoreStrategy::extract3DVector(py::dict &dict, const char *key,
                                               QVector3D &toSet, const QString &parentType,
                                                 const QString &name,
                                               const QString &identifier,
                                               QList<QString> &invalidData) {
    if (dict.contains(key)) {
        py::object tItem = dict[key];
        if (py::isinstance<py::list>(tItem)) {
            py::list t = py::list(tItem);
            int tSize = t.size();
            if (tSize == 3) {
                float tConverted[3];
                for (int j = 0; j < tSize; j++) {
                    if (py::isinstance<py::float_>(t[j])) {
                        tConverted[j] = t[j].cast<float>();
                    } else {
                        qDebug() << "Translation vector contains non-float values "
                                    "(ID:" << identifier << "). Skipping " + parentType + ".";
                        invalidData.append(identifier);
                        continue;
                    }
                }
                toSet = QVector3D(tConverted[0],
                                  tConverted[1],
                                  tConverted[2]);
                return true;
            } else {
                qDebug() << name + " does not contain exactly 3 floats "
                            "(ID:" << identifier << "). Skipping " + parentType + ".";
                invalidData.append(identifier);
                return false;
            }
        } else {
            qDebug() << "Return value is not a list of floats "
                        "(ID:" << identifier << "). Skipping " + parentType + ".";
            invalidData.append(identifier);
            return false;
        }
    } else {
        qDebug() << name + " \"" + key + "\" not in return dict"
                    " (ID:" << identifier << "). Skipping " + parentType + ".";
        invalidData.append(identifier);
        return false;
    }

}

bool PythonLoadAndStoreStrategy::extractFloat(py::dict &dict, const char *key,
                                              float &toSet, float defaultValue) {
    if (dict.contains(key)) {
        toSet = dict[key].cast<float>();
    } else {
        toSet = defaultValue;
    }
    return true;
}

QList<ImagePtr> PythonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        Q_EMIT error(tr("The script does not exist."));
        return images;
    }

    try {
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
                    if (!extractID(itemDict, "img_id", imageID, "image", i)) {
                        continue;
                    }
                    if (!extractPath(itemDict, "img_path", imagePath,
                                     "image", imageID, m_imagesWithInvalidData,
                                     true)) {
                        continue;
                    }
                    if (!extractPath(itemDict, "base_path", basePath,
                                     "image", imageID, m_imagesWithInvalidData,
                                     true)) {
                        continue;
                    }
                    if (!extractPath(itemDict, "segmentation_image_path", segmentationImagePath,
                                     "image", imageID, m_imagesWithInvalidData,
                                     false)) {
                        continue;
                    }
                    if (!extract3x3Matrix(itemDict, "K", cameraMatrix, "image", "Camera matrix",
                                          imagePath, m_imagesWithInvalidData)) {
                        continue;
                    }
                    extractFloat(itemDict, "near_plane", nearPlane, NEAR_PLANE);
                    extractFloat(itemDict, "far_plane", farPlane, FAR_PLANE);
                    images.append(ImagePtr(new Image(imageID, imagePath, basePath,
                                                     cameraMatrix, nearPlane, farPlane)));
                } else {
                    qDebug() << "Return value for image is not a dict. Skipping image.";
                    m_imagesWithInvalidData.append(QString::number(i));
                    continue;
                }
            }
        } else if (py::isinstance<py::str>(result)) {
            QString message = "Failed to load images. "
                              "The script produced an error while "
                              "trying to load images: ";
            message += QString::fromStdString(result.cast<std::string>());
            Q_EMIT error(tr(message.toStdString().c_str()));
        } else {
            Q_EMIT error(tr("Failed to load images. Return value for images is "
                            "not a list of dicts. No images were loaded."));
        }
    } catch (py::error_already_set &e) {
        QString message = "Failed to load images. The script produced "
                          "an error while trying to load images: ";
        message += QString::fromUtf8(e.what());
        Q_EMIT error(tr(message.toStdString().c_str()));
    }
    if (m_imagesWithInvalidData.size() > 0) {
        Q_EMIT error(tr("There were images with invalid data."));
    }
    return images;
}

QList<ObjectModelPtr> PythonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModelPtr> objectModels;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        Q_EMIT error(tr("The script does not exist."));
        return objectModels;
    }

    try {
        py::object result = script.attr("load_object_models")(m_objectModelsPath.toUtf8().data());

        if (py::isinstance<py::list>(result)) {
            py::list result_list = py::list(result);
            int length = result_list.size();
            for (int i = 0; i < length; i++) {
                py::object item = result_list[i];
                if (py::isinstance<py::dict>(item)) {
                    py::dict itemDict = py::dict(item);
                    QString objID, objectModelPath, basePath;
                    if (!extractID(itemDict, "obj_id", objID, "object model", i)) {
                        continue;
                    }
                    if (!extractPath(itemDict, "obj_model_path", objectModelPath,
                                     "object model", objID, m_objectModelsWithInvalidData,
                                     true)) {
                        continue;
                    }
                    if (!extractPath(itemDict, "base_path", basePath,
                                     "object model", objID, m_objectModelsWithInvalidData,
                                     true)) {
                        continue;
                    }
                    objectModels.append(ObjectModelPtr(new ObjectModel(objID, objectModelPath, basePath)));
                } else {
                    qDebug() << "Return value for object model is not a dict. Skipping object model.";
                    m_objectModelsWithInvalidData.append(QString::number(i));
                    continue;
                }
            }
        } else if (py::isinstance<py::str>(result)) {
            QString message = "Failed to load object models. "
                              "The script produced an error while "
                              "trying to load object models: ";
            message += QString::fromStdString(result.cast<std::string>());
            Q_EMIT error(tr(message.toStdString().c_str()));
        } else {
            Q_EMIT error(tr("Failed to load object models. "
                            "Return value for object models is not a list of dicts. "
                            "No object models were loaded."));
        }
    } catch (py::error_already_set &e) {
        QString message = "Failed to load object models. "
                          "The script produced an error while "
                          "trying to load object models: ";
        message += QString::fromUtf8(e.what());
        Q_EMIT error(tr(message.toStdString().c_str()));
    }
    if (m_objectModelsWithInvalidData.size() > 0) {
        Q_EMIT error(tr("There were object models with invalid data."));
    }
    return objectModels;
}

bool PythonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {
    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        Q_EMIT error(tr("The script does not exist."));
        return false;
    }

    try {
        QString poseIdD = objectImagePose.id();
        QString imageID = objectImagePose.image()->id();
        QString objID = objectImagePose.objectModel()->id();
        QString imagePath = objectImagePose.image()->imagePath();
        QString objectModelPath = objectImagePose.objectModel()->path();
        py::list rotation;
        // Transposed because QMatrix3x3 transposes it when loading from the float array
        const float* rotationValues = objectImagePose.rotation().toRotationMatrix().transposed().constData();
        for (int i = 0; i < 9; i++) {
            rotation.append(rotationValues[i]);
        }
        py::list translation;
        for (int i = 0; i < 3; i++) {
            translation.append(objectImagePose.position()[0]);

        }
        py::object result = script.attr("persist_pose")(m_posesFilePath.toStdString(),
                                                        poseIdD.toStdString(),
                                                        imageID.toStdString(),
                                                        imagePath.toStdString(),
                                                        objID.toStdString(),
                                                        objectModelPath.toStdString(),
                                                        rotation,
                                                        translation,
                                                        deletePose);
        if (py::isinstance<py::bool_>(result)) {
            return result.cast<bool>();
        } else if (py::isinstance<py::str>(result)) {
            QString message = "Failed to persist a pose. "
                              "The script produced an error while "
                              "trying to persist a pose: ";
            message += QString::fromStdString(result.cast<std::string>());
            Q_EMIT error(tr(message.toStdString().c_str()));
        } else {
            Q_EMIT error(tr("Failed to persist a pose. The script return an unkown return type."));
        }
    } catch (py::error_already_set &e) {
        QString message = "Failed to persist a pose. "
                          "The script produced an error while "
                          "trying to persist a pose: ";
        message += QString::fromUtf8(e.what());
        Q_EMIT error(tr(message.toStdString().c_str()));
    }

    return false;
}

QList<PosePtr> PythonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images,
                                                     const QList<ObjectModelPtr> &objectModels) {
    QList<PosePtr> poses;

    QFileInfo fileInfo(m_loadSaveScript);
    if (!fileInfo.exists()) {
        Q_EMIT error(tr("The script does not exist."));
        return poses;
    }

    // For faster lookup by ID
    QMap<QString, ImagePtr> imagesForID;
    QMap<QString, ImagePtr> imagesForPath;
    Q_FOREACH(ImagePtr image, images) {
        if (imagesForID.contains(image->id())) {
            qDebug() << "Two images with the same ID found, this should never happen.";
        }
        imagesForID[image->id()] = image;
        imagesForPath[image->imagePath()] = image;
    }
    QMap<QString, ObjectModelPtr> objectModelsForID;
    QMap<QString, ObjectModelPtr> objectModelsForPath;
    Q_FOREACH(ObjectModelPtr objectModel, objectModels) {
        if (objectModelsForID.contains(objectModel->id())) {
            qDebug() << "Two object models with the same ID found, this should never happen.";
        }
        objectModelsForID[objectModel->id()] = objectModel;
        objectModelsForPath[objectModel->path()] = objectModel;
    }

    try {
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
                            if (!extractIDOrPathAndRetrieve(poseDict, "img_id", "img_path",
                                                            i, j, image, m_imagesWithInvalidData,
                                                            imagesForID, imagesForPath, "pose")) {
                                continue;
                            }
                            if (!extractIDOrPathAndRetrieve(poseDict, "obj_id", "obj_path",
                                                            i, j, objectModel, m_objectModelsWithInvalidData,
                                                            objectModelsForID, objectModelsForPath, "pose")) {
                                continue;
                            }
                            // Can only do this at the end since we need the image and object model
                            // to create an ID if we don't receive one
                            if (poseDict.contains("pose_id")) {
                                py::object poseIDItem = poseDict["pose_id"];
                                if (py::isinstance<py::str>(poseIDItem)) {
                                    poseID = QString::fromStdString(poseDict["pose_id"].cast<std::string>());
                                } else {
                                    qDebug() << "\"poseID\" in return dict but is not "
                                                "a string (Index: "  << i << ", " << j <<  "). Skipping pose.";
                                    m_posesWithInvalidData.append(QString::number(i) + ", " +
                                                                  QString::number(j));
                                    continue;
                                }
                            } else {
                                // No error, if no ID is present, create one
                                poseID = GeneralHelper::createPoseId(*image, *objectModel);
                            }
                            // Get rotation
                            if (!extract3x3Matrix(poseDict, "R", rotation, "pose", "Rotation matrix",
                                                  poseID, m_posesWithInvalidData)) {
                                continue;
                            }
                            if (!extract3DVector(poseDict, "t", translation, "pose", "Translation vector",
                                                  poseID, m_posesWithInvalidData)) {
                                continue;
                            }
                            // Get translation
                            poses.append(PosePtr(new Pose(poseID, translation, rotation, image, objectModel)));
                        } else {
                            qDebug() << "Return value for pose is not "
                                        "a dict (Index:" << i << ", " << j << "). Skipping pose.";
                        }
                    }
                } else {
                    qDebug() << "Return value of poses for image is not "
                                "a list of dicts. Skipping image.";
                }
            }
        } else if (py::isinstance<py::str>(result)) {
            QString message = "Failed to load poses. "
                              "The script produced an error while "
                              "trying to load poses: ";
            message += QString::fromStdString(result.cast<std::string>());
            Q_EMIT error(tr(message.toStdString().c_str()));
        } else {
            Q_EMIT error(tr("Failed to load poses. "
                            "Return value for poses is not a list of dicts. "
                            "No poses were loaded."));
        }
    } catch (py::error_already_set &e) {
        QString message = "Failed to load poses. "
                          "The script produced an error while "
                          "trying to load poses: ";
        message += QString::fromUtf8(e.what());
        Q_EMIT error(tr(message.toStdString().c_str()));
    }
    if (m_posesWithInvalidData.size() > 0) {
        Q_EMIT error(tr("There were poses with invalid data."));
    }
    return poses;
}

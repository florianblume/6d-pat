#ifndef PYTHONLOADANDSTORESTRATEGY_H
#define PYTHONLOADANDSTORESTRATEGY_H

#include "model/loadandstorestrategy.hpp"

#include <QObject>
#include <pybind11/pybind11.h>

namespace py = pybind11;

class PythonLoadAndStoreStrategy : public LoadAndStoreStrategy {

    Q_OBJECT

public:
    PythonLoadAndStoreStrategy();

    ~PythonLoadAndStoreStrategy();

    void applySettings(SettingsPtr settings) override;

    bool persistPose(const Pose &objectImagePose, bool deletePose) override;

    QList<ImagePtr> loadImages() override;

    QList<ObjectModelPtr> loadObjectModels() override;

    QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                             const QList<ObjectModelPtr> &objectModels) override;

private:
    bool extractPath(py::dict &dict, const char *key,
                     QString &toSet, const QString &type,
                     const QString &identifier,
                     QList<QString> &invalidData,
                     bool required);
    bool extractID(py::dict &dict, const char *key,
                   QString &toSet, const QString &type,
                   const int defaultID);
    template<class T>
    bool extractIDOrPathAndRetrieve(py::dict &dict, const char *id_key,
                                    const char *path_key,
                                    int i, int j, T &itemToSet,
                                    QList<QString> &invalidData,
                                    QMap<QString, T> itemsForID,
                                    QMap<QString, T> itemsForPath,
                                    const QString &type);
    bool extract3x3Matrix(py::dict &dict, const char *key,
                          QMatrix3x3 &toSet, const QString &parentType,
                          const QString &name,
                          const QString &identifier,
                          QList<QString> &invalidData);
    bool extract3DVector(py::dict &dict, const char *key,
                       QVector3D &toSet, const QString &parentType,
                         const QString &name,
                       const QString &identifier,
                       QList<QString> &invalidData);
    bool extractFloat(py::dict &dict, const char *key,
                      float &toSet, float defaultValue);

private:
    QString m_loadSaveScript;
    py::module m_sys;
    py::module m_script;
    bool m_scriptInitialized = false;
};

typedef QSharedPointer<PythonLoadAndStoreStrategy> PythonLoadAndStoreStrategyPtr;

#endif // PYTHONLOADANDSTORESTRATEGY_H

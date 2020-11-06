#ifndef POSEEDITINGMODEL_H
#define POSEEDITINGMODEL_H

#include "model/pose.hpp"
#include "model/image.hpp"

#include <QObject>
#include <QMetaObject>

class PoseEditingModel : public QObject
{
    Q_OBJECT

public:
    explicit PoseEditingModel(QObject *parent = nullptr);
    void selectPose(PosePtr pose);
    PosePtr selectedPose();

Q_SIGNALS:
    void selectedPoseChanged(PosePtr selected, PosePtr deselected);
    void posePositionChanged(QVector3D position);
    void poseRotationChanged(QQuaternion rotation);

private:
    PosePtr m_selectedPose;
};

#endif // POSEEDITINGMODEL_H

#ifndef POSEEDITINGMODEL_H
#define POSEEDITINGMODEL_H

#include "model/pose.hpp"
#include "model/image.hpp"

#include <QObject>
#include <QMetaObject>

class PoseEditingController : public QObject
{
    Q_OBJECT

public:
    explicit PoseEditingController(QObject *parent = nullptr);
    void selectPose(PosePtr pose);
    PosePtr selectedPose();

Q_SIGNALS:
    void selectedPoseChanged(PosePtr selected, PosePtr deselected);
    void poseValuesChanged(PosePtr pose);

private Q_SLOTS:
    void onPosePositionChanged(QVector3D position);
    void onPoseRotationChanged(QQuaternion rotation);

private:
    PosePtr m_selectedPose;
};

#endif // POSEEDITINGMODEL_H

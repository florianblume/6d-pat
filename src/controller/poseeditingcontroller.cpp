#include "poseeditingcontroller.hpp"

PoseEditingController::PoseEditingController(QObject *parent) : QObject(parent) {
}

void PoseEditingController::selectPose(PosePtr pose) {
    if (pose == m_selectedPose) {
        // Selecting the same pose again deselects it
        m_selectedPose.reset();
        Q_EMIT selectedPoseChanged(PosePtr(), pose);
    } else {
        if (!m_selectedPose.isNull()) {
            disconnect(m_selectedPose.get(), &Pose::positionChanged,
                       this, &PoseEditingController::onPosePositionChanged);
            disconnect(m_selectedPose.get(), &Pose::rotationChanged,
                       this, &PoseEditingController::onPoseRotationChanged);
        }
        PosePtr oldPose = m_selectedPose;
        m_selectedPose = pose;
        connect(m_selectedPose.get(), &Pose::positionChanged,
                this, &PoseEditingController::onPosePositionChanged);
        connect(m_selectedPose.get(), &Pose::rotationChanged,
                this, &PoseEditingController::onPoseRotationChanged);
        Q_EMIT selectedPoseChanged(m_selectedPose, oldPose);
    }
}

PosePtr PoseEditingController::selectedPose() {
    return m_selectedPose;
}

void PoseEditingController::onPosePositionChanged(QVector3D /*position*/) {
    Q_EMIT poseValuesChanged(m_selectedPose);
}

void PoseEditingController::onPoseRotationChanged(QQuaternion /*rotation*/) {
    Q_EMIT poseValuesChanged(m_selectedPose);
}

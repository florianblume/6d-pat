#include "poseeditingmodel.hpp"

PoseEditingModel::PoseEditingModel(QObject *parent) : QObject(parent) {
}

void PoseEditingModel::selectPose(PosePtr pose) {
    if (pose == m_selectedPose) {
        // Selecting the same pose again deselects it
        m_selectedPose.reset();
        Q_EMIT selectedPoseChanged(PosePtr(), pose);
    } else {
        if (!m_selectedPose.isNull()) {
            disconnect(m_selectedPose.get(), &Pose::positionChanged,
                       this, &PoseEditingModel::posePositionChanged);
            disconnect(m_selectedPose.get(), &Pose::rotationChanged,
                       this, &PoseEditingModel::poseRotationChanged);
        }
        PosePtr oldPose = m_selectedPose;
        m_selectedPose = pose;
        connect(m_selectedPose.get(), &Pose::positionChanged,
                this, &PoseEditingModel::posePositionChanged);
        connect(m_selectedPose.get(), &Pose::rotationChanged,
                this, &PoseEditingModel::poseRotationChanged);
        Q_EMIT selectedPoseChanged(m_selectedPose, oldPose);
    }
}

PosePtr PoseEditingModel::selectedPose() {
    return m_selectedPose;
}

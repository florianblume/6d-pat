#ifndef POSEEDITINGVIEW_H
#define POSEEDITINGVIEW_H

#include "model/image.hpp"
#include "model/pose.hpp"

class PoseEditingView {

public:
    virtual void setImages(const QList<Image> &images) = 0;
    virtual void setSelectedImage(int index) = 0;
    virtual void setPoses(const QList<Pose> &poses) = 0;
    virtual void addPose(const Pose &pose) = 0;
    virtual void removePose(const Pose &pose) = 0;
    virtual void setSelectedPose(int index) = 0;
    virtual void setClicks(const QList<QVector3D> &clicks) = 0;
    virtual void onSelectedPoseValuesChanged(const Pose &pose) = 0;
    virtual void onPosesSaved() = 0;
    virtual void onPoseCreationAborted() = 0;
    virtual void reset() = 0;
};

#endif // POSEEDITINGVIEW_H

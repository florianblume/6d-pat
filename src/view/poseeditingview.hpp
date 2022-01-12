#ifndef POSEEDITINGVIEW_H
#define POSEEDITINGVIEW_H

#include "model/image.hpp"
#include "model/pose.hpp"

class PoseEditingView {

public:
    virtual void setImages(const QList<Image> &images) = 0;
    virtual void setSelectedImage(const Image &image) = 0;
    virtual void setPoses(const QList<Pose> &poses) = 0;
    virtual void addPose(const Pose &pose) = 0;
    virtual void removePose(const Pose &pose) = 0;
    virtual void setSelectedPose(const Pose &pose) = 0;
    virtual void deselectPose() = 0;
    virtual void onSelectedPoseValuesChanged(const Pose &pose) = 0;
    virtual void onPosesSaved() = 0;
    virtual void onPoseCreationAborted() = 0;
    virtual void reset() = 0;

Q_SIGNALS:
    void poseChanged(const Pose &pose);
};

#endif // POSEEDITINGVIEW_H

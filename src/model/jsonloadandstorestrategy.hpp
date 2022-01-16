#ifndef TEXTFILELOADANDSTORESTRATEGY_H
#define TEXTFILELOADANDSTORESTRATEGY_H

#include "loadandstorestrategy.hpp"
#include <QString>
#include <QStringList>
#include <QList>
#include <QFileSystemWatcher>

/*!
 * \brief The TextFileLoadAndStoreStrategy class is a simple implementation of a LoadAndStoreStrategy that makes no use of
 * caching already loaded images or object models and writes poses to text files. That is why it is important to set the
 * proper path to the folder of poses before using this strategy.
 */
class JsonLoadAndStoreStrategy : public LoadAndStoreStrategy {

    Q_OBJECT

public:
    /*!
     * \brief TextFileLoadAndStoreStrategy Constructor of this strategy. The paths MUST be set aferwards to use it
     * properly, otherwise the strategy won't deliver any content.
     */
    JsonLoadAndStoreStrategy();

    ~JsonLoadAndStoreStrategy();

    bool persistPose(const Pose &pose, bool deletePose) override;

    QList<Image> loadImages() override;

    ObjectModelList loadObjectModels() override;

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses are stored depends on the
     * strategy.
     *
     * IMPORTANT: This implementation of LoadAndStoreStrategy makes use of text files to store poses, this means that the
     * path to the folder has to be set before this method is called. Failing to do so will raise an exception.
     *
     * \param images the images to insert as references into the respective poses
     * \param objectModels the object models to insert as reference into the respective pose
     * \param poses the list that the corresondences are to be added to
     * \return the list of all stored poses
     * \throws an exception if the path to the folder that should hold the poses has not been set previously
     */
    QList<Pose> loadPoses(const QList<Image> &images,
                          const ObjectModelList &objectModels) override;
};

typedef QSharedPointer<JsonLoadAndStoreStrategy> JsonLoadAndStoreStrategyPtr;

#endif // TEXTFILELOADANDSTORESTRATEGY_H

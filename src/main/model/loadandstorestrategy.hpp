#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "correspondence.hpp"
#include "image.hpp"
#include "objectmodel.hpp"
#include <QObject>
#include <QString>
#include <QList>
#include <QDir>

using namespace std;

/*!
 * \brief The LoadAndStoreStrategy class provides the actual storing implementation that the model manager
 * uses to load entities. This way multiple implementations are possible, e.g. one writing to text files
 * or one wirting to a database.
 */
class LoadAndStoreStrategy : public QObject {

    Q_OBJECT

public:

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief persistObjectImageCorrespondence Persists the given ObjectImageCorrespondence. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImageCorrespondence the object image correspondence to persist
     * \param deleteCorrespondence indicates whether the correspondence should be persistently deleted
     * \return true if persisting the object image correspondence was successful, false if not
     */
    virtual bool persistObjectImageCorrespondence(Correspondence *objectImageCorrespondence,
                                                  bool deleteCorrespondence) = 0;

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual QList<Image> loadImages() = 0;

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual QList<ObjectModel> loadObjectModels() = 0;

    /*!
     * \brief loadCorrespondences Loads the correspondences at the given path. How the correspondences
     * are stored depends on the strategy.
     * \return the list of all stored correspondences
     */
    virtual QList<Correspondence> loadCorrespondences(const QList<Image> &images,
                                                      const QList<ObjectModel> &objectModels) = 0;

    /*!
     * \brief pathExists Checks whether the given path exists on the file system and is accessible.
     * \param path the path that is to be checked
     * \return true if the path exists and is accessible, false if not
     */
    bool pathExists(const QDir &path);

Q_SIGNALS:

    void imagesChanged();
    void failedToLoadImages(const QString& message);
    void objectModelsChanged();
    void failedToLoadObjectModels(const QString &message);
    void correspondencesChanged();
    void failedToLoadCorrespondences(const QString &message);
    void failedToPersistCorrespondence(const QString &message);

};

#endif // LOADANDSTORESTRATEGY_H

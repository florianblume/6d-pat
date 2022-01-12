#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <QString>
#include <QSharedPointer>

//! Class ObjectModel stores the URL to the 3D model of the object.
class ObjectModel {

public:
    ObjectModel();

    //! Constructor of class ObjectModel.
    /*!
      This constructor stores the path to the 3D object model.
      \param objectModelPath the relative path to the object model
      \param basePath the path to the folder (or parent folder) where relative to
      the object model lies
    */
    ObjectModel(const QString &id, const QString& objectModelPath, const QString& basePath);

    /*!
     * \brief ObjectModel copy constructor
     * \param other
     */
    ObjectModel(const ObjectModel &other);

    /*!
     * \brief id returns the ID of this object model. Can be the index of the object model
     * in the filesystem or some custom ID.
     * \return
     */
    QString id() const;

    //! Returns the path to the object model.
    /*!
      \return the path to the object model
    */
    QString path() const;

    /*!
     * \brief getAbsolutePath Returns the absolute path to the object model, i.e. the base path concatenated with the object model path.
     * \return the absolute path to the object model
     */
    QString absolutePath() const;

    /*!
     * \brief getBasePath Returns the base path to the folder where either the model is direclty located or located within a subfolder.
     * \return the base path
     */
    QString basePath() const;

    bool operator==(const ObjectModel &other);

    ObjectModel& operator=(const ObjectModel &other);

private:
    QString m_id;
    QString m_objectModelPath;
    QString m_basePath;
};

typedef QSharedPointer<ObjectModel> ObjectModelPtr;
typedef QList<ObjectModel> ObjectModelList;

#endif // OBJECTMODEL_H

#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <QString>

//! Class ObjectModel stores the URL to the 3D model of the object.
class ObjectModel {
private:
    QString objectModelPath;
    QString basePath;

public:
    //! Constructor of class ObjectModel.
    /*!
      This constructor stores the path to the 3D object model.
      \param objectModelPath the relative path to the object model
      \param basePath the path to the folder (or parent folder) where relative to
      the object model lies
    */
    ObjectModel(const QString& objectModelPath, const QString& basePath);

    /*!
     * \brief ObjectModel copy constructor
     * \param other
     */
    ObjectModel(const ObjectModel &other);

    //! Returns the path to the object model.
    /*!
      \return the path to the object model
    */
    QString getPath() const;

    /*!
     * \brief getAbsolutePath Returns the absolute path to the object model, i.e. the base path concatenated with the object model path.
     * \return the absolute path to the object model
     */
    QString getAbsolutePath() const;

    /*!
     * \brief getBasePath Returns the base path to the folder where either the model is direclty located or located within a subfolder.
     * \return the base path
     */
    QString getBasePath() const;

    ObjectModel& operator=(const ObjectModel &other);
};

#endif // OBJECTMODEL_H

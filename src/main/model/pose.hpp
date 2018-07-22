#ifndef OBJECTIMAGECORRESPONDENCE_H
#define OBJECTIMAGECORRESPONDENCE_H

#include "image.hpp"
#include "objectmodel.hpp"
#include <QVector3D>
#include <QMatrix3x3>
#include <QString>

//! This class represents a match between an object model and an image, i.e. stores where the object is located on the image and
//! how it is rotated.
class Pose {
private:
    //! The position of the object on the image. The value z is the depth, i.e. how large the object ist.
    QVector3D position;
    //! The rotation of the object on the image.
    QMatrix3x3 rotation;
    //! The image associated with this corresopndence.
    const Image* image;
    //! The object model associated with this pose.
    const ObjectModel* objectModel;
    //! The ID of the pose. This is necessary becuase images might contain some objects multiple times.
    QString id;

public:
    //! Constructor of class ObjectImagePose.
    /*!
     * \brief ObjectImagePose Constructs an ObjectImagePose and assigns the passed values.
     * \param id the ID of the pose
     * \param _image the associated image
     * \param _objectModel the associated object model
     */
    Pose(QString id, QVector3D position, QMatrix3x3 rotation,
                              const Image* image, const ObjectModel* objectModel);

    /*!
     * \brief ObjectImagePose copy constructor for class ObjectImagePose.
     * \param other the pose to copy from
     */
    Pose(const Pose &other);

    /*!
     * \brief getPosition Returns the position of the object on the image. The value z of the position determines the depth,
     * i.e. how big the object is.
     * \return the position of the image
     */
    QVector3D getPosition() const;

    /*!
     * \brief getRotation Returns the rotation of the object on the image.
     * \return the rotation of the image
     */
    QMatrix3x3 getRotation() const;

    /*!
     * \brief getImage Returns the image associated with this pose.
     * \return the image associated with this pose
     */
    const Image* getImage() const;
    /*!
     * \brief getObjectModel Returns the object model associated with this pose.
     * \return the object model associated with this pose
     */
    const ObjectModel* getObjectModel() const;

    void setPosition(QVector3D position);

    void setRotation(QMatrix3x3 rotation);
    /*!
     * \brief getID Returns the unique ID of this pose.
     * \return the unique ID of this pose
     */
    QString getID() const;

    /*!
     * \brief operator == Returns true if the IDs of the two poses are the same.
     * \param objectImagePose the other pose to check for
     * \return true if the IDs are the same, false otherwise
     */
    bool operator==(const Pose& objectImagePose);

    Pose& operator=(const Pose &other);

};

#endif // OBJECTIMAGECORRESPONDENCE_H

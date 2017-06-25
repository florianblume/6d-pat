#ifndef OBJECTIMAGECORRESPONDENCE_H
#define OBJECTIMAGECORRESPONDENCE_H

#include "image.h"
#include "objectmodel.h"
#include "point.h"
#include <string>

using namespace std;

//! This class represents a match between an object model and an image, i.e. stores where the object is located on the image and
//! how it is rotated.
class ObjectImageCorrespondence {
private:
    //! The position of the object on the image. The value z is the depth, i.e. how large the object ist.
    Point position;
    //! The rotation of the object on the image.
    Point rotation;
    //! The indicator whether the user is confident in the correspondence.
    bool accepted;
    //! The image associated with this corresopndence.
    Image* const image;
    //! The object model associated with this correspondence.
    ObjectModel* const objectModel;

public:
    //! Constructor of class ObjectImageCorrespondence.
    /*!
     * \brief ObjectImageCorrespondence Constructs an ObjectImageCorrespondence and assigns the passed values.
     * \param x the x position of the object
     * \param y the y position of the object
     * \param z the z position, i.e. the depth of the object (this determines the size of the object on the image)
     * \param r1 the rotation of the x-axis of the object
     * \param r2 the rotation of the y-axis of the object
     * \param r3 the rotation of the z-axis  of the object
     * \param _image the associated image
     * \param _objectModel the associated object model
     */
    ObjectImageCorrespondence(int x, int y, int z, int r1, int r2, int r3, Image *_image, ObjectModel *_objectModel);

    /*!
     * \brief getPosition Returns the position of the object on the image. The value z of the position determines the depth,
     * i.e. how big the object is.
     * \return the position of the image
     */
    Point getPosition();

    /*!
     * \brief getRotation Returns the rotation of the object on the image.
     * \return the rotation of the image
     */
    Point getRotation();
    /*!
     * \brief getImage Returns the image associated with this correspondence.
     * \return the image associated with this correspondence
     */
    Image* getImage();
    /*!
     * \brief getObjectModel Returns the object model associated with this correspondence.
     * \return the object model associated with this correspondence
     */
    ObjectModel* getObjectModel();
    /*!
     * \brief setPosition Sets the position of the object on the image.
     * \param x the x position of the object
     * \param y the y position of the object
     * \param z the z position, i.e. the depth of the object
     */
    void setPosition(int x, int y, int z);
    /*!
     * \brief setRotation Sets the rotation of the object on the image.
     * \param r1 the rotation of the x-axis of the object
     * \param r2 the rotation of the y-axis of the object
     * \param r3 the rotation of the z-axis of the object
     */
    void setRotation(int r1, int r2, int r3);

};

#endif // OBJECTIMAGECORRESPONDENCE_H

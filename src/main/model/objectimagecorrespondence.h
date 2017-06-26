#ifndef OBJECTIMAGECORRESPONDENCE_H
#define OBJECTIMAGECORRESPONDENCE_H

#include "image.h"
#include "objectmodel.h"
#include "point.h"
#include <string.h>

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
    const Image* const image;
    //! The object model associated with this correspondence.
    const ObjectModel* const objectModel;
    //! The ID of the correspondence. This is necessary becuase images might contain some objects multiple times.
    const string id;

public:
    //! Constructor of class ObjectImageCorrespondence.
    /*!
     * \brief ObjectImageCorrespondence Constructs an ObjectImageCorrespondence and assigns the passed values.
     * \param id the ID of the correspondence
     * \param x the x position of the object
     * \param y the y position of the object
     * \param z the z position, i.e. the depth of the object (this determines the size of the object on the image)
     * \param r1 the rotation of the x-axis of the object
     * \param r2 the rotation of the y-axis of the object
     * \param r3 the rotation of the z-axis  of the object
     * \param _image the associated image
     * \param _objectModel the associated object model
     */
    ObjectImageCorrespondence(string _id, int x, int y, int z, int r1, int r2, int r3, const Image* _image, const ObjectModel* _objectModel);

    /*!
     * \brief getPosition Returns the position of the object on the image. The value z of the position determines the depth,
     * i.e. how big the object is.
     * \return the position of the image
     */
    const Point* getPosition() const;

    /*!
     * \brief getRotation Returns the rotation of the object on the image.
     * \return the rotation of the image
     */
    const Point* getRotation() const;
    /*!
     * \brief getImage Returns the image associated with this correspondence.
     * \return the image associated with this correspondence
     */
    const Image* getImage() const;
    /*!
     * \brief getObjectModel Returns the object model associated with this correspondence.
     * \return the object model associated with this correspondence
     */
    const ObjectModel* getObjectModel() const;
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

    /*!
     * \brief getID Returns the unique ID of this correspondence.
     * \return the unique ID of this correspondence
     */
    const string getID() const;

    /*!
     * \brief operator == Returns true if the IDs of the two correspondences are the same.
     * \param objectImageCorrespondence the other correspondence to check for
     * \return true if the IDs are the same, false otherwise
     */
    bool operator==(ObjectImageCorrespondence objectImageCorrespondence);

    /*!
     * \brief toString Converts this correspondence to a string.
     * \return this correspondence converted to a string
     */
    string toString() const;

};

#endif // OBJECTIMAGECORRESPONDENCE_H

#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <string>

using namespace std;

//! Class ObjectModel stores the URL to the 3D model of the object.
class ObjectModel {
private:
    const string objectModelUrl;

public:
    //! Constructor of class ObjectModel.
    /*!
      This constructor stores the URL to the 3D object model.
      \param _objectModelUrl the URL to the object model
    */
    ObjectModel(string _objectModelUrl);
    //! Returns the URL to the object model.
    /*!
      \return the URL to the object model
    */
    string getObjectModelUrl();
};

#endif // OBJECTMODEL_H

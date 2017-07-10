#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

//! Class ObjectModel stores the URL to the 3D model of the object.
class ObjectModel {
private:
    const path objectModelPath;
    const path basePath;

public:
    //! Constructor of class ObjectModel.
    /*!
      This constructor stores the path to the 3D object model.
      \param _objectModelUrl the path to the object model
    */
    ObjectModel(path _objectModelPath);

    //! Returns the path to the object model.
    /*!
      \return the path to the object model
    */
    const path getPath() const;

    /*!
     * \brief getAbsolutePath Returns the absolute path to the object model, i.e. the base path concatenated with the object model path.
     * \return the absolute path to the object model
     */
    const path getAbsolutePath() const;

    /*!
     * \brief getBasePath Returns the base path to the folder where either the model is direclty located or located within a subfolder.
     * \return the base path
     */
    const path getBasePath() const;
};

#endif // OBJECTMODEL_H

#ifndef MODELMANAGERLISTENER_H
#define MODELMANAGERLISTENER_H

#include "objectimagecorrespondence.hpp"
#include <string>

/*!
 * \brief The ModelManagerListener class provides and interface for a class that wants to be notified of changes within
 * the ModelManager it registered with.
 */
class ModelManagerListener {
public:
    virtual ~ModelManagerListener() {}

    /*!
     * \brief correspondenceAdded Will be called whenever a ObjectImageCorrespondence is added to the manager.
     * \param id the ID of the correspondence that was added
     */
    virtual void correspondenceAdded(string id) = 0;

    /*!
     * \brief correspondenceAdded Will be called whenever a ObjectImageCorrespondence is updated by the manager.
     * \param id the ID of the correspondence that was updated
     */
    virtual void objectImageCorrespondenceUpdated(string id) = 0;

    /*!
     * \brief correspondenceAdded Will be called whenever a ObjectImageCorrespondence is deleted from the manager.
     * \param id the ID of the correspondence that was deleted
     */
    virtual void correspondenceDeleted(string id) = 0;

    /*!
     * \brief imagesChanged Will be called whenever the list of images changes.
     */
    virtual void imagesChanged() = 0;

    /*!
     * \brief objectModelsChanged Will be called whenever the list of object models changes.
     */
    virtual void objectModelsChanged() = 0;

    /*!
     * \brief correspondencesChanged Will be called whenever the list of correspondences changes, i.e. this function
     * will only be called if a correspondence will be deleted or added and therefore changes the list size or if
     * the underlying LoadAndStoreStrategy reports list changes for whatever reason.
     */
    virtual void correspondencesChanged() = 0;

};

#endif // MODELMANAGERLISTENER_H

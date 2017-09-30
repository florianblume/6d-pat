#ifndef LOADANDSTORESTRATEGYLISTENER_H
#define LOADANDSTORESTRATEGYLISTENER_H

/*!
 * \brief The LoadAndStoreStrategyListener class provides an interface for a LoadAndStoreStrategy to call back in
 * case that the list of entities that are persisted somehow changed. This could be for a database-based strategy
 * that someone manually edited the database or for a textfile-based strategy that someone manually deleted one of
 * the textfiles that store information.
 */
class LoadAndStoreStrategyListener
{
public:
    virtual ~LoadAndStoreStrategyListener() {}

    /*!
     * \brief imagesChanged Will be called by the LoadAndStoreStrategy that this listener registered with whenever
     * the list of images changes.
     */
    virtual void imagesChanged() = 0;

    /*!
     * \brief objectModelsChanged Will be called by the LoadAndStoreStrategy that this listener registered with whenever
     * the list of object models changes.
     */
    virtual void objectModelsChanged() = 0;

    /*!
     * \brief corresopndencesChanged Will be called by the LoadAndStoreStrategy that this listener registered with whenever
     * the list of correspondences changes.
     */
    virtual void correspondencesChanged() = 0;
};

#endif // LOADANDSTORESTRATEGYLISTENER_H

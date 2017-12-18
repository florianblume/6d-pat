#ifndef PREFERENCESSTORE_H
#define PREFERENCESSTORE_H

#include "preferences.hpp"
#include "misc/globaltypedefs.h"
#include <QString>
#include <QObject>

class PreferencesStore : public QObject
{
    Q_OBJECT

public:
    PreferencesStore();
    UniquePointer<Preferences> createEmptyPreferences(const QString &identifier);
    void savePreferences(Preferences *preferences);
    UniquePointer<Preferences> loadPreferencesByIdentifier(const QString &identifier);

signals:
    void preferencesChanged(const QString &identifier);
};

#endif // PREFERENCESSTORE_H

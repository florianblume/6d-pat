#ifndef PREFERENCESSTORE_H
#define PREFERENCESSTORE_H

#include "settings.hpp"

#include <QSharedPointer>
#include <QString>
#include <QObject>

class SettingsStore : public QObject
{
    Q_OBJECT

public:
    SettingsStore();
    QSharedPointer<Settings> createEmptyPreferences(const QString &identifier);
    void savePreferences(Settings *settingsPointer);
    QSharedPointer<Settings> loadPreferencesByIdentifier(const QString &identifier);

signals:
    void settingsChanged(const QString &identifier);
};

#endif // PREFERENCESSTORE_H

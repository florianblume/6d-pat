#include "objectmodel.hpp"
#include <QDir>

ObjectModel::ObjectModel() {

}

ObjectModel::ObjectModel(const QString &id, const QString& objectModelPath, const QString& basePath)
    : m_id(id),
      m_objectModelPath(objectModelPath),
      m_basePath(basePath) {
}

ObjectModel::ObjectModel(const ObjectModel &other) {
    m_objectModelPath = other.m_objectModelPath;
    m_basePath = other.m_basePath;
}

QString ObjectModel::path() const {
    return m_objectModelPath;
}

QString ObjectModel::basePath() const {
    return m_basePath;
}

QString ObjectModel::absolutePath() const {
    return QDir(m_basePath).filePath(m_objectModelPath);
}

bool ObjectModel::operator==(const ObjectModel &other) {
    // QString supports standard string comparison ==
    return m_basePath == other.m_basePath && m_objectModelPath == other.m_objectModelPath;
}

ObjectModel& ObjectModel::operator=(const ObjectModel &other) {
    m_basePath = other.m_basePath;
    m_objectModelPath = other.m_objectModelPath;
    return *this;
}

QString ObjectModel::id() const
{
    return m_id;
}

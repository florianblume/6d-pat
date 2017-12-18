#include "loadandstorestrategy.hpp"

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

bool LoadAndStoreStrategy::pathExists(const QDir &path) {
    QDir dir(path);
    return dir.exists();
}

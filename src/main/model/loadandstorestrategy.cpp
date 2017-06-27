#include "loadandstorestrategy.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

bool LoadAndStoreStrategy::pathExists(const string path) {
    boost::filesystem::path _path(path);
    return boost::filesystem::is_directory(_path);
}

#include "loadandstorestrategy.hpp"

LoadAndStoreStrategy::LoadAndStoreStrategy() {
    qRegisterMetaType<LoadAndStoreStrategy::Error>("LoadAndStoreStrategy::Error");
}

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

#ifndef GLOBALTYPEDEFS_H
#define GLOBALTYPEDEFS_H

#include <memory>
#include <QString>

#define DEPTH_BUFFER_SIZE 24
#define STENCIL_BUFFER_SIZE 0
#define NUMBER_OF_SAMPLES 4

template <class C>
using UniquePointer = std::unique_ptr<C>;

namespace Global {

    static const QString NO_PATH = "none";

}

#endif // GLOBALTYPEDEFS_H

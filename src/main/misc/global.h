#ifndef GLOBALTYPEDEFS_H
#define GLOBALTYPEDEFS_H

#include <memory>

#define DEPTH_BUFFER_SIZE 32
#define STENCIL_BUFFER_SIZE 8
#define NUMBER_OF_SAMPLES 16

template <class C>
using UniquePointer = std::unique_ptr<C>;

#endif // GLOBALTYPEDEFS_H

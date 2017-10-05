#ifndef NNGCPP_INTEGRATION_H
#define NNGCPP_INTEGRATION_H

// extern "C" is already utilized when __cplusplus is defined.
#include <nng/nng.h>

//// TODO: TBD: doing this here for now; really should be part of the CMakeLists.txt however...
// #define NNGCPP_STATIC_LIB

#if defined(_WIN32) || defined(_WIN64)
    #if defined(NNGCPP_STATIC_LIB)
        #define NNGCPP_DECLSPEC extern
    #else
        #if defined(NNGCPP_SHARED_LIB)
            #define NNGCPP_DECLSPEC __declspec(dllexport)
        #else // NNGCPP_SHARED_LIB
            #define NNGCPP_DECLSPEC __declspec(dllimport)
        #endif // NNGCPP_SHARED_LIB
    #endif
#endif

#if defined(NNGCPP_SHARED_LIB)
#pragma comment(lib, "nng_static.lib")
#endif // defined(NNGCPP_SHARED_LIB)

#include "core/options.h"
#include "core/nng_exception.h"

#endif // NNGCPP_INTEGRATION_H

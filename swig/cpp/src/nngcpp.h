#ifndef NNGCPP_H
#define NNGCPP_H

// extern "C" is already utilized when __cplusplus is defined.
#include <nng.h>

#include "nng/protocol/protocol.h"

// #pragma comment(lib, "nng.lib")

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

#endif // NNGCPP_H

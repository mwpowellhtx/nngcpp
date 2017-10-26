#ifndef NNGCPP_H
#define NNGCPP_H

#ifdef UNIT_TESTING
#include <nng.h>
#else // UNIT_TESTING
#include <nng/nng.h>
#endif // UNIT_TESTING

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

#ifndef NNG_ONLY

#include <core/core.h>
#include <core/async.h>
#include <messaging/messaging.h>
#include <protocol/protocol.h>

#endif // NNG_ONLY

#endif // NNGCPP_H

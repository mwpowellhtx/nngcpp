/*
 * Copyright (c) 2017 Michael W Powell <mwpowellhtx@gmail.com>
 *
 * This software is supplied under the terms of the MIT License, a
 * copy of which should be located in the distribution where this
 * file was obtained (LICENSE.txt).  A copy of the license may also be
 * found online at https://opensource.org/licenses/MIT.
 */

#ifndef CHRONO_HELPERS_HPP
#define CHRONO_HELPERS_HPP

#include <chrono>

#ifndef CAST_DURATION_TO_MS
#define CAST_DURATION_TO_MS(x) std::chrono::duration_cast<std::chrono::milliseconds>(x)
#endif // CAST_DURATION_TO_MS

#endif // CHRONO_HELPERS_HPP

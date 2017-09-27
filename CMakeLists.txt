#
#   Copyright (c) 2012 Martin Sustrik  All rights reserved.
#   Copyright (c) 2013 GoPivotal, Inc.  All rights reserved.
#   Copyright (c) 2015-2016 Jack R. Dunaway. All rights reserved.
#   Copyright 2016 Franklin "Snaipe" Mathieu <franklinmathieu@gmail.com>
#   Copyright 2017 Garrett D'Amore <garrett@damore.org>
#   Copyright (c) 2017 Michael W. Powell <mwpowellhtx@gmail.com> All rights reserved.
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom
#   the Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#   IN THE SOFTWARE.
#
cmake_minimum_required (VERSION 3.1)

project (cppnng CXX)

find_package (git)

if (GIT_FOUND)
    message (STATUS "git found: ${GIT_EXECUTABLE} ${GIT_VERSION_STRING}")
else
    message (FATAL_ERROR "git required: not found")
endif()

set (GIT_SUBMODULE_NNG_REPO_DIR repo/nng)

macro (GitSubmoduleUpdateNng REVISION)

    if (REVISION EQUAL "")
        set (REVISION HEAD)
    endif # (REVISION EQUAL "")

    exec_program (git .
        ARGS submodule update --init --recursive
        RETURN_VALUE GIT_REVAL
        )

endmacro (GitSubmoduleUpdateNng)

GitSubmoduleUpdateNng ()

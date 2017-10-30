/*
  Copyright (c) 2017 Michael W. Powell <mwpowellhtx@gmail.com> All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

%{
#include "cpp/src/transport/views/IFamilyView.h"
%}

%module(directors="1") Transport;

%include "std_vector.i"
%include "csharp/src/Core/Types.i"

namespace nng {

/* We must hard-code the enumerated values in the C++ source.
Just be sure to document where they came from. */
%typemap(csbase) SocketAddressFamily "ushort"
%rename("Unspecified") af_unspec;
%rename("InProcess") af_inproc;
%rename("InterProcess") af_ipc;
%rename("IPv4") af_inet;
%rename("IPv6") af_inet6;
%rename("ZeroTier") af_zt;

%typemap(csclassmodifiers) IAddrFamilyViewBase "public abstract partial class"
%typemap(csinterfaces) IAddrFamilyViewBase "IHaveOne, IAddressFamilyView, IEquatable<AddressFamilyViewBase>"

%rename("AddressFamilyViewBase") IAddrFamilyViewBase;

%ignore IAddrFamilyViewBase::operator==;
%ignore IAddrFamilyViewBase::operator!=;

// Rename methods which caused conflicts with internally seen included definitions.
%rename("GetPort") IAddrFamilyViewBase::__GetPort;
%rename("SetPort") IAddrFamilyViewBase::__SetPort;

}

%include "cpp/src/transport/views/IFamilyView.h"

SWIG_STD_VECTOR_ENHANCED(nng::uint8_t)
%template("IPv6AddrByteVector") std::vector<nng::uint8_t>;

SWIG_STD_VECTOR_ENHANCED(nng::uint16_t)
%template("IPv6AddrUShortVector") std::vector<nng::uint16_t>;

SWIG_STD_VECTOR_ENHANCED(nng::uint32_t)
%template("IPv6AddrUIntVector") std::vector<nng::uint32_t>;

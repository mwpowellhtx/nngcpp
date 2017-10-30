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
#include "cpp/src/transport/address.h"
%}

%module(directors="1") Transport;

%include "std_string.i"
%include "csharp/src/Core/Types.i"

namespace nng {

%ignore _SockAddrFamilyNames;

%typemap(csimports) _SockAddr %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _SockAddr "public partial class"
%typemap(csinterfaces) _SockAddr "IHaveOne, ISocketAddress, IEquatable<SocketAddress>"

%ignore _SockAddr::get;
%ignore _SockAddr::operator==;
%ignore _SockAddr::operator!=;

%csmethodmodifiers _SockAddr::GetView "protected"
%csmethodmodifiers _SockAddr::GetFamily "protected"
%csmethodmodifiers _SockAddr::SetFamily "protected"

%typemap(cscode) _SockAddr %{
  public IAddressFamilyView View {
    get { return GetView(); }
  }

  public ushort Family {
    get { return GetFamily(); }
    set { SetFamily(value); }
  }
%}

%rename("CopyFrom") _SockAddr::operator=;
%csmethodmodifiers _SockAddr::operator= "protected"

%rename("SocketAddress") _SockAddr;

}

%include "cpp/src/transport/address.h"

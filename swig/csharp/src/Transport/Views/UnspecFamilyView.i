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
#include "cpp/src/transport/views/unspec_family_view.h"
%}

%module(directors="1") Transport;

%include "csharp/src/Core/Types.i"

namespace nng {

%ignore align_view(const _SockAddr&);

%typemap(csimports) _UnspecFamilyView %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _UnspecFamilyView "public partial class";
%typemap(csinterfaces) _UnspecFamilyView "IHaveOne, IAddressFamilyView, IEquatable<IAddressFamilyView>";
// Yes, clear out the Base Class, which serves nothing. Instead expose via Interface.
%typemap(csbase) _UnspecFamilyView "";

// TODO: TBD: "equatable" may not be something that gets easily exposed after all... at least not via SWIG, and not without a little during the mapping...
%ignore _UnspecFamilyView::operator==;
%ignore _UnspecFamilyView::operator!=;
%ignore _UnspecFamilyView::Equals;

//// TODO: TBD: should not need these modifer changes any longer.
//%csmethodmodifiers _UnspecFamilyView::GetIPv4Addr "public override";
//%csmethodmodifiers _UnspecFamilyView::SetIPv4Addr "public override";
//%csmethodmodifiers _UnspecFamilyView::GetIPv6Addr "public override";
//%csmethodmodifiers _UnspecFamilyView::GetIPv6Addr16 "public override";
//%csmethodmodifiers _UnspecFamilyView::GetIPv6Addr32 "public override";
//%csmethodmodifiers _UnspecFamilyView::SetIPv6Addr "public override";
//%csmethodmodifiers _UnspecFamilyView::SetIPv6Addr16 "public override";
//%csmethodmodifiers _UnspecFamilyView::SetIPv6Addr32 "public override";
//%csmethodmodifiers _UnspecFamilyView::__GetPort "public override";
//%csmethodmodifiers _UnspecFamilyView::__SetPort "public override";

// Rename methods which caused conflicts with internally seen included definitions.
%rename("GetPort") _UnspecFamilyView::__GetPort;
%rename("SetPort") _UnspecFamilyView::__SetPort;

%rename("UnspecifiedFamilyView") _UnspecFamilyView;

}

%include "cpp/src/transport/views/unspec_family_view.h"

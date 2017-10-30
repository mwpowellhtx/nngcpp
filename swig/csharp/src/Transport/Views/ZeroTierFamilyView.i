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
#include "cpp/src/transport/views/zt_family_view.h"
%}

%module(directors="1") Transport;

%include "csharp/src/Core/Types.i"

namespace nng {

%ignore align_view(const _SockAddr&);

%typemap(csimports) _ZeroTierFamilyView %{
using System;
using System.Runtime.InteropServices;
%}

%typemap(csclassmodifiers) _ZeroTierFamilyView "public partial class";
%typemap(csinterfaces) _ZeroTierFamilyView "IHaveOne, IAddressFamilyView, IEquatable<IAddressFamilyView>";
%typemap(csbase) _ZeroTierFamilyView "";

// TODO: TBD: ditto Equatable mappings...
%ignore _ZeroTierFamilyView::operator==;
%ignore _ZeroTierFamilyView::operator!=;
%ignore _ZeroTierFamilyView::Equals;

//// TODO: TBD: should not need these modifer changes any longer.
//%csmethodmodifiers _ZeroTierFamilyView::GetIPv4Addr "public override";
//%csmethodmodifiers _ZeroTierFamilyView::SetIPv4Addr "public override";
//%csmethodmodifiers _ZeroTierFamilyView::GetIPv6Addr "public override";
//%csmethodmodifiers _ZeroTierFamilyView::GetIPv6Addr16 "public override";
//%csmethodmodifiers _ZeroTierFamilyView::GetIPv6Addr32 "public override";
//%csmethodmodifiers _ZeroTierFamilyView::SetIPv6Addr "public override";
//%csmethodmodifiers _ZeroTierFamilyView::SetIPv6Addr16 "public override";
//%csmethodmodifiers _ZeroTierFamilyView::SetIPv6Addr32 "public override";
//%csmethodmodifiers _ZeroTierFamilyView::__GetPort "public override";
//%csmethodmodifiers _ZeroTierFamilyView::__SetPort "public override";

// Rename methods which caused conflicts with internally seen included definitions.
%rename("GetPort") _ZeroTierFamilyView::__GetPort;
%rename("SetPort") _ZeroTierFamilyView::__SetPort;

%rename("ZeroTierFamilyView") _ZeroTierFamilyView;

}

%include "cpp/src/transport/views/zt_family_view.h"

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
#include "cpp/src/options/names.h"
%}

%module(directors="1") Options;

%include "std_string.i"

// TODO: TBD: possible to expose properties as true-static consts?
// TODO: TBD: at minimum, static readonly fields
// TODO: TBD: also, possible to do static classes?
namespace nng {

%typemap(csimports) _OptionNames %{
using System;
using System.Runtime.InteropServices;
%}

%rename("OptionNames") _OptionNames;

%rename("SocketName") _OptionNames::socket_name;
%rename("CompatDomain") _OptionNames::compat_domain;
%rename("Raw") _OptionNames::raw;
%rename("LingerDuration") _OptionNames::linger_duration;
%rename("ReceiveBuffer") _OptionNames::recv_buf;
%rename("SendBuffer") _OptionNames::send_buf;
%rename("ReceiveFileDescriptor") _OptionNames::recv_fd;
%rename("SendFileDescriptor") _OptionNames::send_fd;
%rename("ReceiveTimeoutDuration") _OptionNames::recv_timeout_duration;
%rename("SendTimeoutDuration") _OptionNames::send_timeout_duration;
%rename("LocalAddress") _OptionNames::local_addr;
%rename("RemoteAddress") _OptionNames::remote_addr;
%rename("Url") _OptionNames::url;
%rename("MaxTtl") _OptionNames::max_ttl;
%rename("Protocol") _OptionNames::protocol;
%rename("Transport") _OptionNames::transport;
%rename("MaxReceiveSize") _OptionNames::max_recv_sz;
%rename("MinReconnectDuration") _OptionNames::min_reconnect_duration;
%rename("MaxReconnectDuration") _OptionNames::max_reconnect_duration;
%rename("PairV1Polyamorous") _OptionNames::pair1_polyamorous;
%rename("RequestResendDuration") _OptionNames::req_resend_duration;
%rename("SubscriberSubscribe") _OptionNames::sub_subscribe;
%rename("SubscriberUnsubscribe") _OptionNames::sub_unsubscribe;
%rename("SurveyorSurveyDuration") _OptionNames::surveyor_survey_duration;

}

%include "cpp/src/options/names.h"

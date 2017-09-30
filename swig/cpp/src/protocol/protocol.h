
#ifndef CPPNNG_PROT_H
#define CPPNNG_PROT_H

#include "../core/core.h"

#define CPPNNG_PROT_NAMESPACE nng::protocol

#define CPPNNG_PROT_V0_NAMESPACE nng::protocol::v0
#define CPPNNG_PROT_V1_NAMESPACE nng::protocol::v1

#include "nng/protocol/bus/bus.h"

#include "nng/protocol/pair/pair_v0.h"
#include "nng/protocol/pair/pair_v1.h"

#include "nng/protocol/pipeline/pull.h"
#include "nng/protocol/pipeline/push.h"

#include "nng/protocol/pubsub/pub.h"
#include "nng/protocol/pubsub/sub.h"

#include "nng/protocol/reqrep/req.h"
#include "nng/protocol/reqrep/rep.h"

#include "nng/protocol/survey/respond.h"
#include "nng/protocol/survey/survey.h"

#endif // CPPNNG_PROT_H

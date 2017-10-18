#ifndef CPPNNG_PROT_H
#define CPPNNG_PROT_H

#define CPPNNG_PROT_NAMESPACE nng::protocol

#define CPPNNG_PROT_V0_NAMESPACE nng::protocol::v0
#define CPPNNG_PROT_V1_NAMESPACE nng::protocol::v1

#include "bus/bus.h"

#include "pair/pair_v0.h"
#include "pair/pair_v1.h"

#include "pipeline/pull.h"
#include "pipeline/push.h"

#include "pubsub/pub.h"
#include "pubsub/sub.h"

#include "reqrep/req.h"
#include "reqrep/rep.h"

#include "survey/respond.h"
#include "survey/survey.h"

#endif // CPPNNG_PROT_H

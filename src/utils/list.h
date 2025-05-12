#ifndef C_MINILIB_SIP_CODEC_LIST_H
#define C_MINILIB_SIP_CODEC_LIST_H

#include <stdint.h>
#include <sys/queue.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

STAILQ_HEAD(cmsc_ViaList, cmsc_Field_Via);

STAILQ_HEAD(cmsc_AcceptList, cmsc_Field_Accept);

#endif // C_MINILIB_SIP_CODEC_LIST_H

#ifndef C_MINILIB_SIP_CODEC_INVITE_H
#define C_MINILIB_SIP_CODEC_INVITE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_sip_proto_invite_init(struct cmsc_Scheme **scheme);
void cmsc_sip_proto_invite_destroy(struct cmsc_Scheme **scheme);

#endif // C_MINILIB_SIP_CODEC_INVITE_H

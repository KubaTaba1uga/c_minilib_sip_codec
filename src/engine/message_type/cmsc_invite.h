#ifndef C_MINILIB_SIP_CODEC_INVITE_H
#define C_MINILIB_SIP_CODEC_INVITE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_engine_invite_scheme_init(struct cmsc_Scheme **scheme);
void cmsc_engine_invite_scheme_destroy(struct cmsc_Scheme **scheme);

#endif // C_MINILIB_SIP_CODEC_INVITE_H

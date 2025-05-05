#ifndef C_MINILIB_SIP_CODEC_MESSAGE_H
#define C_MINILIB_SIP_CODEC_MESSAGE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_message_create(struct cmsc_SipMessage **message);

void cmsc_message_destroy(struct cmsc_SipMessage **message);

#endif // C_MINILIB_SIP_CODEC_SCHEME_H

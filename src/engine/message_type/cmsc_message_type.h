#ifndef C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H
#define C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_message_type_parse(const uint32_t buffer_len,
                                    const char *buffer,
                                    enum cmsc_SipMsgType *msg_type);

cme_error_t cmsc_message_type_generate(const enum cmsc_SipMsgType msg_type,
                                       uint32_t *buffer_len, char *buffer);

#endif // C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H

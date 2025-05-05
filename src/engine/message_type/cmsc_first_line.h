#ifndef C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H
#define C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_first_line_parse(const uint32_t buffer_len, const char *buffer,
                                  const cmsc_sipmsg_t msg);

cme_error_t cmsc_first_line_generate(const cmsc_sipmsg_t msg,
                                     uint32_t *buffer_len, char *buffer);

#endif // C_MINILIB_SIP_CODEC_MESSAGE_TYPE_H

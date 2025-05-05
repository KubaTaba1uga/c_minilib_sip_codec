#ifndef C_MINILIB_SIP_CODEC_ENGINE_H
#define C_MINILIB_SIP_CODEC_ENGINE_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_engine_init(void);

void cmsc_engine_destroy(void);

cme_error_t cmsc_engine_parse(const uint32_t n, const char *buffer,
                              cmsc_sipmsg_t msg);

cme_error_t cmsc_engine_parse_first_line(const uint32_t n, const char *buffer,
                                         cmsc_sipmsg_t msg);

#endif // C_MINILIB_SIP_CODEC_ENGINE_H

/* Goal of sip_proto is to hold all schemes declared by sip modules. 
   When we are parsing or generating message sip_proto uses it's schemes 
   to provide appropriate logic.
*/
#ifndef C_MINILIB_SIP_CODEC_SIP_PROTO_H
#define C_MINILIB_SIP_CODEC_SIP_PROTO_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_sip_proto_init(void);

void cmsc_sip_proto_destroy(void);

cme_error_t cmsc_sip_proto_parse(const uint32_t n, const char *buffer,
                              cmsc_sipmsg_t msg);

#endif // C_MINILIB_SIP_CODEC_SIP_PROTO_H

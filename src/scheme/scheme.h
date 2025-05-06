#ifndef C_MINILIB_SIP_CODEC_SCHEME_H
#define C_MINILIB_SIP_CODEC_SCHEME_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

cme_error_t cmsc_scheme_create(const enum cmsc_SipMsgType msg_type,
                               const char *msg_type_str,
                               struct cmsc_Scheme **scheme);

void cmsc_scheme_destroy(struct cmsc_Scheme **scheme);

cme_error_t
cmsc_scheme_add_mandatory_field(const struct cmsc_SchemeField *field,
                                struct cmsc_Scheme *scheme);

cme_error_t cmsc_scheme_add_optional_field(const struct cmsc_SchemeField *field,
                                           struct cmsc_Scheme *scheme);

#endif // C_MINILIB_SIP_CODEC_SCHEME_H

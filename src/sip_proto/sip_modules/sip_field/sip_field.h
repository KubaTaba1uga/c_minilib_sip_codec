#ifndef C_MINILIB_SIP_CODEC_SIP_FIELD_H
#define C_MINILIB_SIP_CODEC_SIP_FIELD_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_proto/sip_modules/sip_field/parse/parse.h"

// This field can re reused by any scheme
static struct cmsc_SchemeField cmsc_field_to = {
    .id = "To",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_to,
    .generate_field_func = NULL,
};

#endif // C_MINILIB_SIP_CODEC_SIP_FIELD_H

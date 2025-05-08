#ifndef C_MINILIB_SIP_CODEC_SIP_FIELD_H
#define C_MINILIB_SIP_CODEC_SIP_FIELD_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_proto/sip_modules/sip_field/parse/parse.h"

static struct cmsc_SchemeField cmsc_field_to = {
    .id = "To",
    .is_list = false,
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_to,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_from = {
    .id = "From",
    .is_list = false,
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_from,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_cseq = {
    .id = "CSeq",
    .is_list = false,
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_cseq,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_callid = {
    .id = "Call-ID",
    .is_list = false,
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_callid,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_max_forwards = {
    .id = "Max-Forwards",
    .is_list = false,
    .is_field_func = NULL,
    .parse_field_func = cmsc_parse_field_func_max_forwards,
    .generate_field_func = NULL,
};

#endif // C_MINILIB_SIP_CODEC_SIP_FIELD_H

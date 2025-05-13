#ifndef C_MINILIB_SIP_CODEC_COMMON_SUPPORTED_MESSAGES_H
#define C_MINILIB_SIP_CODEC_COMMON_SUPPORTED_MESSAGES_H

#include "parser/parse_field/parse_to.h"
#include "scheme/scheme.h"

static struct cmsc_SchemeField cmsc_field_to = {
    .id = "To",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_to,
    .generate_field_func = NULL,
};

#endif

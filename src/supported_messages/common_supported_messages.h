#ifndef C_MINILIB_SIP_CODEC_COMMON_SUPPORTED_MESSAGES_H
#define C_MINILIB_SIP_CODEC_COMMON_SUPPORTED_MESSAGES_H

#include "parser/parse_field/parse_accept.h"
#include "parser/parse_field/parse_allow.h"
#include "parser/parse_field/parse_call_id.h"
#include "parser/parse_field/parse_cseq.h"
#include "parser/parse_field/parse_expires.h"
#include "parser/parse_field/parse_from.h"
#include "parser/parse_field/parse_max_forwards.h"
#include "parser/parse_field/parse_organization.h"
#include "parser/parse_field/parse_subject.h"
#include "parser/parse_field/parse_to.h"
#include "parser/parse_field/parse_user_agent.h"
#include "parser/parse_field/parse_via_l.h"
#include "scheme/scheme.h"

static struct cmsc_SchemeField cmsc_field_accept = {
    .id = "Accept",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_accept,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_allow = {
    .id = "Allow",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_allow,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_call_id = {
    .id = "Call-ID",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_call_id,
    .generate_field_func = NULL,
};
static struct cmsc_SchemeField cmsc_field_cseq = {
    .id = "CSeq",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_cseq,
    .generate_field_func = NULL,
};
static struct cmsc_SchemeField cmsc_field_expires = {
    .id = "Expires",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_expires,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_from = {
    .id = "From",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_from,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_max_forwards = {
    .id = "Max-Forwards",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_max_forwards,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_organization = {
    .id = "Organization",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_organization,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_subject = {
    .id = "Subject",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_subject,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_user_agent = {
    .id = "User-Agent",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_user_agent,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_via = {
    .id = "Via",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_via,
    .generate_field_func = NULL,
};

static struct cmsc_SchemeField cmsc_field_to = {
    .id = "To",
    .is_field_func = NULL,
    .parse_field_func = cmsc_parser_parse_to,
    .generate_field_func = NULL,
};

#endif

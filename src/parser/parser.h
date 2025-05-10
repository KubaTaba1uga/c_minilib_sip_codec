/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include <asm-generic/errno-base.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/common_parser.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_request_line.h"
#include "parser/parse_field/parse_sip_proto_ver.h"
#include "parser/parse_field/parse_status_line.h"
#include "parser/parse_field/parse_supported_msg.h"
#include "scheme/scheme.h"
#include "scheme/scheme_register.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"
#include "utils/string.h"

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  struct cmsc_DynamicBuffer content;
};

static inline cme_error_t
cmsc_parser_parse_msgempty(struct cmsc_HeaderIterator *header_iter,
                           cmsc_parser_t parser, bool *is_next) {

  cme_error_t err;

  if (!parser->msg) {
    if ((err = cmsc_sipmsg_create(&parser->msg))) {
      goto error_out;
    }
  }

  if (!cmsc_headeriter_next(&parser->content, header_iter)) {
    return 0;
  }

  // According RFC 3261 25 request line looks like this:
  //    Method SP Request-URI SP SIP-Version CRLF
  // and response line looks like this:
  //    SIP-Version SP Status-Code SP Reason-Phrase CRLF

  // First let's find sip version
  const char *sip_version =
      cmsc_strnstr(header_iter->line_start, "SIP/", header_iter->line_len);
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version in the line");
    goto error_out;
  }

  // Now let's find supported msg id
  const char *supported_msg_id = NULL;
  if ((err = cmsc_parser_parse_supported_msg(header_iter->line_len,
                                             header_iter->line_start,
                                             parser->msg, &supported_msg_id))) {
    goto error_out;
  }

  cmsc_sipmsg_mark_field_present(parser->msg, cmsc_SupportedFields_IS_REQUEST);
  parser->msg->is_request = supported_msg_id < sip_version;

  struct cmsc_Field_SipProtoVer *proto_ver;
  if (parser->msg->is_request) {
    proto_ver = &parser->msg->request_line.sip_proto_ver;
    if ((err = cmsc_parser_parse_request_line(header_iter->line_end -
                                                  supported_msg_id,
                                              supported_msg_id, parser->msg))) {
      goto error_out;
    }

  } else {
    proto_ver = &parser->msg->status_line.sip_proto_ver;

    if ((err = cmsc_parser_parse_status_line(header_iter->line_end -
                                                 supported_msg_id,
                                             supported_msg_id, parser->msg))) {
      goto error_out;
    }
  }

  if ((err = cmsc_parser_parse_sip_proto_ver(
           (header_iter->line_end - sip_version), sip_version, proto_ver))) {
    goto error_out;
  };

  *is_next = true;

  return NULL;

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsc_parser_parse_headers(struct cmsc_HeaderIterator *header_iter,
                          cmsc_parser_t parser, bool *is_next) {
  struct cmsc_ValueIterator value_iter;
  cme_error_t err;

  struct cmsc_Scheme *scheme =
      cmsc_schemes_register_get_scheme(parser->msg->supmsg);
  if (!scheme) {
    err = cme_errorf(ENOENT, "No scheme matching `msg->supmsg=%d`",
                     parser->msg->supmsg);
    goto error_out;
  }

  // If we are unable to fill one line then we can drop parsing and wait for
  // more data.
  struct cmsc_SchemeField *field;
  uint32_t i;

  if ((err = cmsc_valueiter_init(&value_iter))) {
    goto error_out;
  }

  while (cmsc_headeriter_next(&parser->content, header_iter) &&
         cmsc_valueiter_next(header_iter, &value_iter)) {
    bool is_match = false;
    CMSC_FOREACH_SCHEME_MANDATORY(scheme, i, field) {
      printf("Parsing header: %s, %.*s\n", field->id, header_iter->line_len,
             header_iter->line_start);

      if (field->is_field_func) {
        is_match = field->is_field_func(value_iter.header_len,
                                        value_iter.header_start);
      } else {
        is_match = cmsc_default_is_field_func(
            value_iter.header_len, value_iter.header_start, field->id);
      }

      if (is_match) {
        if ((err = field->parse_field_func(&value_iter, parser->msg))) {
          goto error_out;
        }
        break;
      }
    }

    /* if (is_match) { */
    /*   continue; */
    /* } */

    /* CMSC_FOREACH_SCHEME_OPTIONAL(scheme, i, field) { */
    /*   if (field->is_field_func) { */
    /*     is_match = field->is_field_func(value_iter.header_len, */
    /*                                     value_iter.header_start); */
    /*   } else { */
    /*     is_match = cmsc_default_is_field_func( */
    /*         value_iter.header_len, value_iter.header_start, field->id); */
    /*   } */

    /*   if (is_match) { */
    /*     if ((err = field->parse_field_func( */
    /*              value_iter.value_end - value_iter.value_start, */
    /*              value_iter.value_start, parser->msg))) { */
    /*       goto error_out; */
    /*     } */
    /*     break; */
    /*   } */
    /* } */

    if (!is_match) {
      break;
    }
  }

  return NULL;

error_out:
  return cme_return(err);
}

#endif

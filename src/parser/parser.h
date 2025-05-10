/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include <stdint.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/common_parser.h"
#include "parser/iterator/line_iterator.h"
#include "parser/parse_field/parse_request_line.h"
#include "parser/parse_field/parse_sip_proto_ver.h"
#include "parser/parse_field/parse_status_line.h"
#include "parser/parse_field/parse_supported_msg.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"
#include "utils/string.h"

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  struct cmsc_DynamicBuffer content;
};

// This function basically parse first line
static inline cme_error_t cmsc_parser_parse_msgempty(cmsc_parser_t parser,
                                                     bool *is_next) {
  struct cmsc_HeaderIterator header_iter;
  cme_error_t err;

  if ((err = cmsc_headeriter_init(&header_iter))) {
    goto error_out;
  }

  // If we are unable to fill one line then we can drop parsing and wait for
  // more data.
  *is_next = cmsc_headeriter_next(&parser->content, &header_iter);
  if (!*is_next) {
    return NULL;
  }
  // Let's set default value for all errors etc.
  *is_next = false;

  // According RFC 3261 25 request line looks like this:
  //    Method SP Request-URI SP SIP-Version CRLF
  // and response line looks like this:
  //    SIP-Version SP Status-Code SP Reason-Phrase CRLF

  // First let's find sip version
  const char *sip_version =
      cmsc_strnstr(header_iter.line_start, "SIP/", header_iter.line_len);
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version in the line");
    goto error_out;
  }

  // Now let's find supported msg id
  const char *supported_msg_id = NULL;
  if ((err = cmsc_parser_parse_supported_msg(header_iter.line_len,
                                             header_iter.line_start,
                                             parser->msg, &supported_msg_id))) {
    goto error_out;
  }

  cmsc_sipmsg_mark_field_present(parser->msg, cmsc_SupportedFields_IS_REQUEST);
  parser->msg->is_request = supported_msg_id < sip_version;

  struct cmsc_Field_SipProtoVer *proto_ver;
  if (parser->msg->is_request) {
    proto_ver = &parser->msg->request_line.sip_proto_ver;

    if ((err = cmsc_parser_parse_status_line(header_iter.line_end -
                                                 supported_msg_id,
                                             supported_msg_id, parser->msg))) {
      goto error_out;
    }
  } else {
    proto_ver = &parser->msg->status_line.sip_proto_ver;

    if ((err = cmsc_parser_parse_request_line(header_iter.line_end -
                                                  supported_msg_id,
                                              supported_msg_id, parser->msg))) {
      goto error_out;
    }
  }

  if ((err = cmsc_parser_parse_sip_proto_ver(
           (header_iter.line_end - sip_version), sip_version, proto_ver))) {
    goto error_out;
  };

  return NULL;

error_out:
  return cme_return(err);
}

#endif

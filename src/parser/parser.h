/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/common_parser.h"
#include "parser/iterator/line_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"
#include <stdint.h>
#include <string.h>

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  struct cmsc_DynamicBuffer content;
};

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

  *is_next = false;

  // According RFC 3261 25 request line looks like this:
  //    Method SP Request-URI SP SIP-Version CRLF
  // and response line looks like this:
  //    SIP-Version SP Status-Code SP Reason-Phrase CRLF

  // First let's decide whether we need to parse request line or response line
  const uint32_t line_size = header_iter.line_end - header_iter.line_start;
  const char *sip_version =
      cmsc_strnstr(header_iter.line_start, "SIP/", line_size);
  if (!sip_version) {
    return NULL;
  }

  const char *first_space =
      cmsc_strnstr(header_iter.line_start, " ", line_size);
  if (!first_space) {
    return NULL;
  }

  if ((err = cmsc_parser_parse_sip_proto_ver(line_size, header_iter.line_start,
                                             parser->msg))) {
    return NULL;
  };

  cmsc_sipmsg_mark_field_present(parser->msg, cmsc_SupportedFields_IS_REQUEST);
  parser->msg->is_request = first_space > sip_version;

  for (uint32_t i = cmsc_SupportedMessages_NONE + 1;
       i < cmsc_SupportedMessages_MAX; i++) {
    if (cmsc_strnstr(header_iter.line_start,
                     cmsc_dump_supported_messages_string(i), line_size)) {
      cmsc_sipmsg_mark_field_present(parser->msg,
                                     cmsc_SupportedFields_SUPPORTED_MSG);
      parser->msg->supmsg = i;
      *is_next = true;
      break;
    }
  }

  return NULL;

error_out:
  return cme_return(err);
}

#endif

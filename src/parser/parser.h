/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include "c_minilib_error.h"
#include "parser/iterator/line_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"
#include <stdint.h>

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  // Parser is initialized at
  struct cmsc_DynamicBuffer content;
};

/* static inline char *cmsc_parse_sip_version(char *data, uint32_t data_len, )
 */

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

  // According RFC 3261 25 request line looks like this:
  //    Method SP Request-URI SP SIP-Version CRLF
  // and response line looks like this:
  //    SIP-Version SP Status-Code SP Reason-Phrase CRLF

  // First let's decide whether we need to parse request line or response line
  const uint32_t line_size = header_iter.line_end - header_iter.line_start;
  const char *sip_version =
      cmsc_strnstr(header_iter.line_start, "SIP/", line_size);
  if (!sip_version) {
    *is_next = false;
    return NULL;
  }

  const char *first_space =
      cmsc_strnstr(header_iter.line_start, " ", line_size);
  if (!first_space) {
    *is_next = false;
    return NULL;
  }

  parser->msg->is_request = first_space > sip_version;

  return 0;

error_out:
  return cme_return(err);
}

#endif

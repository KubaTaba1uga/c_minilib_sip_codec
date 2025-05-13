/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_REQUEST_LINE_H
#define C_MINILIB_SIP_CODEC_REQUEST_LINE_H
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/parse_field/parse_sip_proto_ver.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

static inline cme_error_t
cmsc_parser_parse_request_line(const uint32_t buffer_len, const char *buffer,
                               cmsc_sipmsg_t *msg) {
  /*
    According RFC 3261 25 request line looks like this:
      Method SP Request-URI SP SIP-Version CRLF
  */
  cme_error_t err;

  const char *method_end = buffer;
  uint32_t method_len = 0;

  while (isalpha(*method_end)) {
    if (method_len >= buffer_len) {
      break;
    }
    method_len++;
    method_end++;
  }

  if (!isspace(*method_end)) {
    err = cme_error(EINVAL, "No space after method in request line");
    goto error_out;
  }

  const char *sip_version =
      cmsc_strnstr(method_end, "SIP/", buffer_len - method_len);
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version in request line");
    goto error_out;
  }

  if ((err = cmsc_parser_parse_sip_proto_ver(
           (buffer + buffer_len) - sip_version, sip_version,
           &(*msg)->request_line.sip_proto_ver))) {
    goto error_out;
  }

  const char *request_uri = method_end + 1;
  uint32_t request_uri_len = sip_version - request_uri - 1;

  (*msg)->request_line.request_uri =
      cmsc_sipmsg_insert_str(request_uri_len, request_uri, msg);

  cmsc_sipmsg_mark_field_present(*msg, cmsc_SupportedFields_REQUEST_LINE);

  return NULL;
error_out:
  return cme_return(err);
};

#endif

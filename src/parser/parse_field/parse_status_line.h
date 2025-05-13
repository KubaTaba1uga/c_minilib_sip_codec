/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_STATUS_LINE_H
#define C_MINILIB_SIP_CODEC_STATUS_LINE_H
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
cmsc_parser_parse_status_line(const uint32_t buffer_len, const char *buffer,
                              cmsc_sipmsg_t *msg) {
  /*
    According RFC 3261 25 status line looks like this:
       SIP-Version SP Status-Code SP Reason-Phrase CRLF
  */
  cme_error_t err;

  const char *sip_version_end = buffer;
  uint32_t sip_version_len = 0;

  while (isgraph(*sip_version_end)) {
    sip_version_len++;
    sip_version_end++;
  }

  if (!isspace(*sip_version_end)) {
    err = cme_error(EINVAL, "No space after sip_version in status line");
    goto error_out;
  }

  if ((err = cmsc_parser_parse_sip_proto_ver(
           sip_version_len, buffer, &(*msg)->status_line.sip_proto_ver))) {
    goto error_out;
  }

  const char *status_code_end = sip_version_end + 1;
  uint32_t status_code_len = 0;

  while (isdigit(*status_code_end)) {
    if (status_code_len >= buffer_len) {
      break;
    }
    status_code_len++;
    status_code_end++;
  }

  if (!isspace(*status_code_end)) {
    err = cme_error(EINVAL, "No space after status_code in status line");
    goto error_out;
  }

  (*msg)->status_line.status_code = atoi(sip_version_end);
  (*msg)->status_line.reason_phrase =
      cmsc_sipmsg_insert_str((buffer + buffer_len - 2) - (status_code_end + 1),
                             status_code_end + 1, msg);

  cmsc_sipmsg_mark_field_present(*msg, cmsc_SupportedFields_STATUS_LINE);

  return NULL;
error_out:
  return cme_return(err);
};

#endif

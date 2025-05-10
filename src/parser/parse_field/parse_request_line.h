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
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

static inline cme_error_t
cmsc_parser_parse_request_line(const uint32_t buffer_len, const char *buffer,
                               cmsc_sipmsg_t msg) {
  cme_error_t err;

  const char *first_space = cmsc_strnstr(buffer, " ", buffer_len);
  if (!first_space) {
    err = cme_error(EINVAL, "Malformed request line");
    goto error_out;
  }

  first_space++;

  const char *second_space =
      cmsc_strnstr(first_space, " ", (buffer + buffer_len - 1) - first_space);
  if (!second_space) {
    err = cme_error(EINVAL, "Malformed request line");
    goto error_out;
  }

  msg->request_line.request_uri =
      cmsc_sipmsg_insert_str(second_space - first_space, first_space, &msg);

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_REQUEST_LINE);

  return NULL;
error_out:
  return cme_return(err);
};

#endif

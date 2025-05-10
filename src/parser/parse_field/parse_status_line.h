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
#include "sipmsg/sipmsg.h"

static inline cme_error_t
cmsc_parser_parse_status_line(const uint32_t buffer_len, const char *buffer,
                              cmsc_sipmsg_t msg) {
  const char *reason_phrase = NULL;
  cme_error_t err;
  for (uint32_t i = 0; i < buffer_len; i++) {
    if (!isalpha(buffer[i])) {
      continue;
    }

    reason_phrase = buffer + i;
    break;
  }

  if (!reason_phrase) {
    err = cme_error(EINVAL, "No reason phrase in status line");
    goto error_out;
  }

  msg->status_line.reason_phrase =
      cmsc_sipmsg_insert_str(reason_phrase - buffer, buffer, &msg);

  msg->status_line.status_code = atoi(buffer);

  return NULL;
error_out:
  return cme_return(err);
};

#endif

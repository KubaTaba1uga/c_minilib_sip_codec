/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline cme_error_t cmsc_parse_sip_headers(struct cmsc_SipMessage *msg) {
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  const char *max_char = msg->_buf.buf + msg->_buf.len;
  const char *current_char = msg->_buf.buf;
  const char *line_start = msg->_buf.buf;
  struct cmsc_SipHeader *header = NULL;
  uint32_t clrf_counter = 0;

  while (current_char != max_char) {
    switch (*current_char) {
    case ':': {
      if (!header) {
        header = calloc(1, sizeof(struct cmsc_SipHeader));
        if (!header) {
          err = cme_error(ENOMEM, "Cannot allocate memory for `header`");
          goto error_out;
        }

        header->key.buf = line_start;
        header->key.len = current_char - line_start;
        clrf_counter = 0;
      }
      break;
    }
    case '\n': {
      if (*(current_char - 1) == '\r') {
        if (header) {
          header->value.buf = header->key.buf + header->key.len + 1;
          header->value.len = (current_char - header->value.buf) - 1;
          STAILQ_INSERT_TAIL(&msg->sip_headers, header, _next);
          header = NULL;
        }

        clrf_counter++;
        line_start = current_char + 1;
      }
      break;
    }
    }

    current_char++;
  }

  // If not full line parsed we need to clean it up
  if (clrf_counter == 0 && header) {
    free(header);
  }

  (void)clrf_counter;

  return 0;

error_out:
  return cme_return(err);
}

#endif

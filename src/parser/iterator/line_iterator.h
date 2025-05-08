/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_LINE_ITERATOR_H
#define C_MINILIB_SIP_CODEC_LINE_ITERATOR_H

#include <stdint.h>

#include "c_minilib_error.h"
#include "utils/dynamic_buffer.h"
#include "utils/string.h"

struct cmsc_HeaderIterator {
  const char *line_start;
  const char *line_end;
  const char *colon;
};

static inline cme_error_t
cmsc_headeriter_init(struct cmsc_HeaderIterator *headeriter) {
  cme_error_t err;

  if (!headeriter) {
    err = cme_error(EINVAL, "`charbuffer` and `headeriter` cannot be NULL");
    goto error_out;
  }

  headeriter->line_start = NULL;
  headeriter->line_end = NULL;
  headeriter->colon = NULL;

  return 0;

error_out:
  return cme_return(err);
};

// This function return true if it filled headeriter
static inline bool
cmsc_headeriter_next(struct cmsc_DynamicBuffer *buffer,
                     struct cmsc_HeaderIterator *headeriter) {
  if (!buffer || !headeriter) {
    return false;
  }

  if (!headeriter->line_start) {
    headeriter->line_start = buffer->buf;
  } else {
    // +2 for skipping "\r\n"
    headeriter->line_start = headeriter->line_end + 2;
  }

  const uint32_t available_space = CMSC_DYNBUF_GET_END(buffer) - buffer->buf;

  headeriter->line_end =
      cmsc_strnstr(headeriter->line_start, "\r\n", available_space);

  if (!headeriter->line_end) {
    cmsc_headeriter_init(headeriter);
    goto false_out;
  }

  headeriter->colon =
      cmsc_strnstr(headeriter->line_start, ":",
                   headeriter->line_end - headeriter->line_start);

  if (headeriter->line_end == headeriter->line_start) {
    // We hit the body headeriter wont be usefull for us no more.
    // headeriter is not filled with valid data, we now need to use
    // cmsc_BodyIter.
    goto false_out;
  }

  return true;
false_out:
  cmsc_headeriter_init(headeriter);
  return false;
}

#endif

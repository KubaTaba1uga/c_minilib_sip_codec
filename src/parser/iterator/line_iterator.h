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

#define CMSC_LINE_SET(_start, _end, line)                                      \
  line.start = _start;                                                         \
  line.end = _end;                                                             \
  line.len = _end - _start;

#define CMSC_LINE_TRAVERSE(line, offset)                                       \
  line.start += offset;                                                        \
  line.len -= offset;

#define CMSC_LINE_SET_PTR(_start, _end, line)                                  \
  line->start = _start;                                                        \
  line->end = _end;                                                            \
  line->len = _end - _start;

#define CMSC_LINE_TRAVERSE_PTR(line, offset)                                   \
  line->start += offset;                                                       \
  line->len -= offset;

struct cmsc_LineIterator {
  const char *buf; // This buffer can hold multiple lines
  uint32_t buf_len;
};

struct cmsc_Line {
  const char *start;
  const char *end;
  uint32_t len;
};

static inline cme_error_t
cmsc_line_iterator_init(const char *buf, uint32_t buf_len,
                        struct cmsc_LineIterator *line_iter) {
  cme_error_t err;
  if (!buf || !line_iter || buf_len == 0) {
    err = cme_error(EINVAL, "`buf` and `line_iter` cannot be NULL");
    goto error_out;
  }

  line_iter->buf = buf;
  line_iter->buf_len = buf_len;

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsc_Line *
cmsc_line_iterator_next(struct cmsc_LineIterator *line_iter,
                        struct cmsc_Line *next_line) {
  if (!line_iter || !next_line) {
    goto error_out;
  }

  const char *line_end =
      cmsc_strnstr(line_iter->buf, "\r\n", line_iter->buf_len);
  if (!line_end) {
    goto error_out;
  }

  CMSC_LINE_SET_PTR(line_iter->buf, line_end, next_line);

  // Detect overflow
  if (next_line->len > (line_iter->buf_len - 2)) {
    goto error_out;
  }

  line_iter->buf_len -= next_line->len + 2;
  line_iter->buf += next_line->len + 2;

  return next_line;

error_out:
  return NULL;
}

#endif

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_VALUE_ITERATOR_H
#define C_MINILIB_SIP_CODEC_VALUE_ITERATOR_H

#include <ctype.h>

#include "parser/iterator/line_iterator.h"

struct cmsc_ValueIterator {
  struct cmsc_LineIterator line_iter;
  struct cmsc_Line line;
};

struct cmsc_ValueLine {
  struct cmsc_Line header;
  struct cmsc_Line value;
  bool is_inited;
};

static inline cme_error_t
cmsc_value_iterator_init(const char *buf, uint32_t buf_len,
                         struct cmsc_ValueIterator *value_iter) {

  cme_error_t err;
  if (!buf || !value_iter || buf_len == 0) {
    err = cme_error(EINVAL, "`buf` and `value_iter` cannot be NULL");
    goto error_out;
  }

  memset(value_iter, 0, sizeof(struct cmsc_ValueIterator));

  if ((err = cmsc_line_iterator_init(buf, buf_len, &value_iter->line_iter))) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsc_ValueLine *
_cmsc_value_iterator_next(struct cmsc_ValueIterator *value_iter,
                          struct cmsc_ValueLine *next_value) {
  if (!next_value) {
    goto error_out;
  }

  const char *value_end =
      cmsc_strnstr(value_iter->line.start, ",", value_iter->line.len);
  if (!value_end) {
    value_end = value_iter->line.end;
  }

  // Skip all spaces at the end
  while (isspace(*(value_end - 1))) {
    value_end--;
  }

  if (!next_value->is_inited) {
    const char *header_end =
        cmsc_strnstr(value_iter->line.start, ":", value_iter->line.len);
    if (!header_end) {
      goto error_out;
    }

    CMSC_LINE_SET(value_iter->line.start, header_end, next_value->header);
    CMSC_LINE_TRAVERSE(value_iter->line, next_value->header.len + 1);
  }

  /* Skip all spaces at the beginning */
  do {
    value_iter->line.start++;
  } while (isspace(*value_iter->line.start));

  CMSC_LINE_SET(value_iter->line.start, value_end, next_value->value);
  CMSC_LINE_TRAVERSE(value_iter->line, next_value->value.len + 1);

  printf("ValueIter: parsed header = '%.*s'\n", (int)next_value->header.len,
         next_value->header.start);
  printf("ValueIter: parsed value = '%.*s'\n", (int)next_value->value.len,
         next_value->value.start);

  return next_value;

error_out:
  return NULL;
}

static inline struct cmsc_ValueLine *
cmsc_value_iterator_next(struct cmsc_ValueIterator *value_iter,
                         struct cmsc_ValueLine *next_value) {
  if (!value_iter || !next_value) {
    goto error_out;
  }

  /* Detect end of line */
  if (next_value->value.end && next_value->value.end == value_iter->line.end) {
    next_value->is_inited = false;
  }

  if (!next_value->is_inited) {
    if (!cmsc_line_iterator_next(&value_iter->line_iter, &value_iter->line)) {
      goto error_out;
    }
  }

  if (!_cmsc_value_iterator_next(value_iter, next_value)) {
    goto error_out;
  }

  next_value->is_inited = true;

  return next_value;

error_out:
  return NULL;
}

#endif

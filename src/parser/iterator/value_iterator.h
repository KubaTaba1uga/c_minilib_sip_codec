/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_VALUE_ITERATOR_H
#define C_MINILIB_SIP_CODEC_VALUE_ITERATOR_H

#include <ctype.h>
#include <stdint.h>

#include <c_minilib_error.h>

#include "parser/iterator/line_iterator.h"
#include "utils/dynamic_buffer.h"

struct cmsc_ValueIterator {
  // Value is optional, always ensure it is not null even on hit.
  const char *value_start;
  const char *value_end;
  const char *header_start;
  uint32_t header_len;
};

static inline cme_error_t
cmsc_valueiter_init(struct cmsc_ValueIterator *valueiter) {
  cme_error_t err;

  if (!valueiter) {
    err = cme_error(EINVAL, "`charbuffer` and `headeriter` cannot be NULL");
    goto error_out;
  }

  valueiter->value_start = NULL;
  valueiter->value_end = NULL;
  valueiter->header_start = NULL;
  valueiter->header_len = 0;

  return 0;

error_out:
  return cme_return(err);
};

// This function return true if it filled valueiter
static inline bool
cmsc_valueiter_next(const struct cmsc_HeaderIterator *headeriter,
                    struct cmsc_ValueIterator *valueiter) {
  if (!headeriter || !headeriter->line_start || !headeriter->line_end ||
      !headeriter->colon || !valueiter) {
    return false;
  }

  // This means there is no `,` in this line.
  if (valueiter->value_end && (valueiter->value_end == headeriter->line_end)) {
    return false;
  }

  valueiter->header_start = headeriter->line_start;
  valueiter->header_len = headeriter->colon - headeriter->line_start;

  if (!valueiter->value_start) {
    valueiter->value_end = headeriter->line_end;
    // +1 for skipping ":"
    valueiter->value_start = headeriter->colon + 1;
  } else {
    // +1 for skipping ","
    valueiter->value_start = valueiter->value_end + 1;
  }

  // Skip spaces
  while (valueiter->value_start && isspace(*valueiter->value_start) &&
         (valueiter->value_start != headeriter->line_end)) {
    valueiter->value_start++;
  }

  uint32_t available_space = headeriter->line_end - valueiter->value_start;
  if (available_space == 0) {
    valueiter->value_start = NULL;
    valueiter->value_end = NULL;
  } else {
    const char *value_end =
        cmsc_strnstr(valueiter->value_start, ",", available_space);
    if (value_end) {
      valueiter->value_end = value_end;
    } else {
      valueiter->value_end = headeriter->line_end;
    }
  }

  // Debug print at the end
  if (valueiter->value_start && valueiter->value_end) {
    printf("ValueIter: header='%.*s', value='%.*s'\n", valueiter->header_len,
           valueiter->header_start,
           (int)(valueiter->value_end - valueiter->value_start),
           valueiter->value_start);
  } else {
    printf("ValueIter: header='%.*s', value=<null>\n", valueiter->header_len,
           valueiter->header_start);
  }

  return true;
}

#endif

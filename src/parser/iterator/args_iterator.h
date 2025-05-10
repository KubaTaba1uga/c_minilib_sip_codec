/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H
#define C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H

#include <ctype.h>
#include <stdint.h>

#include <c_minilib_error.h>
#include <stdio.h>

#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/dynamic_buffer.h"
#include "utils/string.h"

struct cmsc_ArgsIterator {
  const char *value;
  uint32_t value_len;

  const char *args_value;
  uint32_t args_value_len;
  const char *args_header;
  uint32_t args_header_len;
};

static inline cme_error_t
cmsc_argsiter_init(struct cmsc_ArgsIterator *argsiter) {
  cme_error_t err;

  if (!argsiter) {
    err = cme_error(EINVAL, "`charbuffer` and `headeriter` cannot be NULL");
    goto error_out;
  }

  argsiter->value = NULL;
  argsiter->value_len = 0;

  argsiter->args_value = NULL;
  argsiter->args_value_len = 0;
  argsiter->args_header = NULL;
  argsiter->args_header_len = 0;
  return 0;

error_out:
  return cme_return(err);
};

// This function return true if it filled argsiter
static inline bool
cmsc_argsiter_next(const struct cmsc_ValueIterator *valueiter,
                   struct cmsc_ArgsIterator *argsiter) {
  if (!valueiter || !valueiter->value_start || !valueiter->value_end ||
      !argsiter) {
    return false;
  }

  // Detect that we reached end of line
  if ((argsiter->value &&
       ((argsiter->value + argsiter->value_len) == valueiter->value_end)) ||
      (argsiter->args_value &&
       ((argsiter->args_value + argsiter->args_value_len) ==
        valueiter->value_end))) {
    return false;
  }

  uint32_t value_len = valueiter->value_end - valueiter->value_start;
  const char *args_start;
  if (!argsiter->args_value) {
    args_start = cmsc_strnstr(valueiter->value_start, ";", value_len);

  } else {
    args_start = argsiter->args_value + argsiter->args_value_len;
  }

  if (!args_start) {
    argsiter->args_value = NULL;
    argsiter->args_value_len = 0;
    argsiter->args_header = NULL;
    argsiter->args_header_len = 0;

    argsiter->value = valueiter->value_start;
    argsiter->value_len = value_len;
    return true;
  }

  args_start++;

  const char *header_end =
      cmsc_strnstr(args_start, "=", valueiter->value_end - args_start);
  if (!header_end) {
    return false;
  }

  const char *args_end =
      cmsc_strnstr(args_start, ";", valueiter->value_end - args_start);
  if (!args_end) {
    args_end = valueiter->value_end;
  }

  argsiter->args_header = args_start;
  argsiter->args_header_len = header_end - args_start;

  argsiter->args_value = ++header_end;
  argsiter->args_value_len = args_end - header_end;

  if (!argsiter->value) {
    argsiter->value = valueiter->value_start;
    argsiter->value_len = --args_start - valueiter->value_start;
  }

  return true;
}
#endif

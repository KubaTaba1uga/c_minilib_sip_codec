/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H
#define C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H

#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/string.h"
#include <string.h>

struct cmsc_ArgsIterator {
  struct cmsc_Line value;
};

struct cmsc_Args {
  struct cmsc_Line arg;
  struct cmsc_Line value;
};

static inline cme_error_t
cmsc_args_iterator_init(const struct cmsc_ValueLine *value,
                        struct cmsc_ArgsIterator *args_iter) {

  cme_error_t err;
  if (!args_iter) {
    err = cme_error(EINVAL, "`args_iter` cannot be NULL");
    goto error_out;
  }

  memset(args_iter, 0, sizeof(struct cmsc_ArgsIterator));

  args_iter->value = value->value;

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsc_Args *
cmsc_args_iterator_next(struct cmsc_Args *args,
                        struct cmsc_ArgsIterator *args_iter) {
  if (!args || !args_iter) {
    goto error_out;
  }

  const char *value_end =
      cmsc_strnstr(args_iter->value.start, ";", args_iter->value.len);
  if (!value_end) {
    value_end = args_iter->value.end;
  }

  const char *value_start =
      cmsc_strnstr(args_iter->value.start, "=", args_iter->value.len);
  if (!value_start || value_start > value_end) {
    value_start = args_iter->value.start;
  }

  args->value.start = value_start;
  args->value.end = value_end;
  args->value.len = args->value.end - args->value.start;

  args_iter->value.start += args->value.len;
  args_iter->value.len -= args->value.len;

  return 0;

error_out:
  return NULL;
}

#endif

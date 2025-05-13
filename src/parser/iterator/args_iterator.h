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
  struct cmsc_ValueLine line;
};

struct cmsc_ArgsLine {
  struct cmsc_Line arg_key;
  struct cmsc_Line arg_value;
  struct cmsc_Line value;
};

// Args iterator take value line and allows for iterating over it splited into
// arg and value.
static inline cme_error_t
cmsc_args_iterator_init(const struct cmsc_ValueLine *value,
                        struct cmsc_ArgsIterator *args_iter) {

  cme_error_t err;
  if (!value || !args_iter) {
    err = cme_error(EINVAL, "`value` and `args_iter` cannot be NULL");
    goto error_out;
  }

  memset(args_iter, 0, sizeof(struct cmsc_ArgsIterator));

  args_iter->line = *value;

  return 0;

error_out:
  return cme_return(err);
}

static inline struct cmsc_ArgsLine *
cmsc_args_iterator_next(struct cmsc_ArgsLine *args,
                        struct cmsc_ArgsIterator *args_iter) {
  if (!args || !args_iter) {
    printf("Args iterator error: NULL input\n");
    goto error_out;
  }

  if (!args->value.start) {
    const char *value_end = cmsc_strnstr(args_iter->line.value.start, ";",
                                         args_iter->line.value.len);
    if (!value_end) {
      value_end = args_iter->line.value.end;
    }

    args->value.start = args_iter->line.value.start;
    args->value.end = value_end;
    args->value.len = args->value.end - args->value.start;

    printf("ArgsIter: Parsed value = '%.*s'\n", (int)args->value.len,
           args->value.start);

    args_iter->line.value.start += args->value.len;
    args_iter->line.value.len -= args->value.len;

    return args;
  }

  // We need to skip `;`
  if (args_iter->line.value.len > 0) {
    args_iter->line.value.start++;
    args_iter->line.value.len--;
  }

  const char *key_end =
      cmsc_strnstr(args_iter->line.value.start, "=", args_iter->line.value.len);
  if (!key_end) {
    printf("ArgsIter: Failed to find '=' in param\n");
    goto error_out;
  }

  const char *value_end =
      cmsc_strnstr(key_end, ";", args_iter->line.value.end - key_end);
  if (!value_end) {
    value_end = args_iter->line.value.end;
  }

  args->arg_key.start = args_iter->line.value.start;
  args->arg_key.end = key_end;
  args->arg_key.len = args->arg_key.end - args->arg_key.start;

  args_iter->line.value.start += args->arg_key.len + 1; // +1 to skip '='
  args_iter->line.value.len -= args->arg_key.len + 1;

  args->arg_value.start = args_iter->line.value.start;
  args->arg_value.end = value_end;
  args->arg_value.len = args->arg_value.end - args->arg_value.start;

  printf("ArgsIter: Parsed param: key = '%.*s', value = '%.*s'\n",
         (int)args->arg_key.len, args->arg_key.start, (int)args->arg_value.len,
         args->arg_value.start);

  args_iter->line.value.start += args->arg_value.len;
  args_iter->line.value.len -= args->arg_value.len;

  return args;

error_out:
  return NULL;
}

#endif

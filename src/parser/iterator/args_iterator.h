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

enum cmsc_ArgsNextResults {
  cmsc_ArgsNextResults_NONE = 0,
  cmsc_ArgsNextResults_VALUE,
  cmsc_ArgsNextResults_ARG,
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

static inline enum cmsc_ArgsNextResults
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

    if (value_end - args_iter->line.value.start == 0) {
      return cmsc_ArgsNextResults_NONE;
    }

    CMSC_LINE_SET(args_iter->line.value.start, value_end, args->value);
    CMSC_LINE_TRAVERSE(args_iter->line.value, args->value.len);

    printf("ArgsIter: Parsed value = '%.*s'\n", (int)args->value.len,
           args->value.start);

    return cmsc_ArgsNextResults_VALUE;
  }

  // We need to skip `;`
  if (args_iter->line.value.len > 0) {
    CMSC_LINE_TRAVERSE(args_iter->line.value, 1);
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

  CMSC_LINE_SET(args_iter->line.value.start, key_end, args->arg_key);
  CMSC_LINE_TRAVERSE(args_iter->line.value, args->arg_key.len + 1);

  CMSC_LINE_SET(args_iter->line.value.start, value_end, args->arg_value);
  CMSC_LINE_TRAVERSE(args_iter->line.value, args->arg_value.len);

  printf("ArgsIter: Parsed param: key = '%.*s', value = '%.*s'\n",
         (int)args->arg_key.len, args->arg_key.start, (int)args->arg_value.len,
         args->arg_value.start);

  return cmsc_ArgsNextResults_ARG;

error_out:
  return cmsc_ArgsNextResults_NONE;
}

#endif

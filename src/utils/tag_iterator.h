/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_ARG_ITERATOR_H
#define C_MINILIB_SIP_CODEC_ARG_ITERATOR_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

struct cmsc_ArgIterator {
  struct cmsc_String buf;
  struct cmsc_String arg_key;
  struct cmsc_String arg_value;
  struct cmsc_String value;
};

enum cmsc_ArgNextResults {
  cmsc_ArgNextResults_NONE = 0,
  cmsc_ArgNextResults_VALUE,
  cmsc_ArgNextResults_ARG,
};

static inline cme_error_t
cmsc_arg_iterator_init(const struct cmsc_String buf,
                       struct cmsc_ArgIterator *arg_iter) {

  cme_error_t err;
  if (!arg_iter) {
    err = cme_error(EINVAL, "`arg_iter` cannot be NULL");
    goto error_out;
  }

  memset(arg_iter, 0, sizeof(struct cmsc_ArgIterator));

  arg_iter->buf = buf;

  return 0;

error_out:
  return cme_return(err);
}

#define CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, offset)                 \
  do {                                                                         \
    const uint32_t skip_n = (current_char) - (arg_iter)->buf.buf + (offset);   \
    (arg_iter)->buf.buf += skip_n;                                             \
    (arg_iter)->buf.len -= skip_n;                                             \
  } while (0)

static inline enum cmsc_ArgNextResults
cmsc_arg_iterator_next(struct cmsc_ArgIterator *arg_iter) {
  memset(&arg_iter->arg_key, 0, sizeof(struct cmsc_String));
  memset(&arg_iter->arg_value, 0, sizeof(struct cmsc_String));

  const char *current_char = arg_iter->buf.buf;
  const char *max_char = arg_iter->buf.buf + arg_iter->buf.len;
  while (current_char != max_char) {
    switch (*current_char) {
    case ';': {
      if (!arg_iter->value.buf) {
        arg_iter->value.buf = arg_iter->buf.buf;
        arg_iter->value.len = current_char - arg_iter->buf.buf;
        CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, 1);
        return cmsc_ArgNextResults_VALUE;
      }

      if (arg_iter->arg_key.buf) {
        arg_iter->arg_value.buf =
            arg_iter->arg_key.buf + arg_iter->arg_key.len + 1;
        arg_iter->arg_value.len = current_char - arg_iter->arg_value.buf;
        CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, 1);
        return cmsc_ArgNextResults_ARG;
      }
      break;
    };
    case '=': {
      if (arg_iter->value.buf && !arg_iter->arg_key.buf) {
        arg_iter->arg_key.buf = arg_iter->buf.buf;
        arg_iter->arg_key.len = current_char - arg_iter->buf.buf;
      }
      break;
    };
    case ',': {
      if (arg_iter->value.buf) {
        memset(&arg_iter->value, 0, sizeof(struct cmsc_String));
        if (arg_iter->arg_key.buf) {
          arg_iter->arg_value.buf =
              arg_iter->arg_key.buf + arg_iter->arg_key.len + 1;
          arg_iter->arg_value.len = current_char - arg_iter->arg_value.buf;
          CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, 1);
          return cmsc_ArgNextResults_ARG;
        }
      }

      break;
    };
    default:;
    }
    current_char++;
  }

  if (!arg_iter->value.buf) {
    arg_iter->value.buf = arg_iter->buf.buf;
    arg_iter->value.len = current_char - arg_iter->buf.buf;
    CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, 1);
    return cmsc_ArgNextResults_VALUE;
  }

  if (arg_iter->arg_key.buf) {
    arg_iter->arg_value.buf = arg_iter->arg_key.buf + arg_iter->arg_key.len + 1;
    arg_iter->arg_value.len = current_char - arg_iter->arg_value.buf;
    CMSC_ARG_ITER_TRAVERSE(arg_iter, current_char, 0);
    return cmsc_ArgNextResults_ARG;
  }

  return cmsc_ArgNextResults_NONE;
}

#undef CMSC_ARG_ITER_TRAVERSE

#endif

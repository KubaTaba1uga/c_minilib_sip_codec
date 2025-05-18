/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_ARG_ITERATOR_H
#define C_MINILIB_SIP_CODEC_ARG_ITERATOR_H

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/bstring.h"

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
  if (!arg_iter) {
    return cme_error(EINVAL, "`arg_iter` cannot be NULL");
  }

  memset(arg_iter, 0, sizeof(struct cmsc_ArgIterator));
  arg_iter->buf = buf;
  return 0;
}

static inline void cmsc_arg_iterator_traverse(struct cmsc_ArgIterator *arg_iter,
                                              const char *current_char,
                                              uint32_t offset) {
  uint32_t skip_n = (uint32_t)(current_char - arg_iter->buf.buf + offset);
  arg_iter->buf.buf += skip_n;
  arg_iter->buf.len -= skip_n;
}

static inline enum cmsc_ArgNextResults
cmsc_arg_iterator_emit_value(struct cmsc_ArgIterator *arg_iter,
                             const char *current_char, uint32_t offset) {
  arg_iter->value.buf = arg_iter->buf.buf;
  arg_iter->value.len = (uint32_t)(current_char - arg_iter->buf.buf);
  if (arg_iter->arg_value.buf && isspace(*arg_iter->arg_value.buf)) {
    arg_iter->arg_value.buf++;
    arg_iter->arg_value.len--;
  }

  cmsc_arg_iterator_traverse(arg_iter, current_char, offset);
  return cmsc_ArgNextResults_VALUE;
}

static inline enum cmsc_ArgNextResults
cmsc_arg_iterator_emit_arg(struct cmsc_ArgIterator *arg_iter,
                           const char *current_char, uint32_t offset) {
  arg_iter->arg_value.buf = arg_iter->arg_key.buf + arg_iter->arg_key.len + 1;
  arg_iter->arg_value.len = (uint32_t)(current_char - arg_iter->arg_value.buf);

  if (arg_iter->arg_value.len > 0 &&
      arg_iter->arg_value.buf[arg_iter->arg_value.len - 1] == ',') {
    arg_iter->arg_value.len--;
  }
  if (arg_iter->arg_value.len > 0 &&
      arg_iter->arg_value.buf[arg_iter->arg_value.len - 1] == ';') {
    arg_iter->arg_value.len--;
  }

  cmsc_arg_iterator_traverse(arg_iter, current_char, offset);
  return cmsc_ArgNextResults_ARG;
}

static inline enum cmsc_ArgNextResults
cmsc_arg_iterator_next(struct cmsc_ArgIterator *arg_iter) {
  memset(&arg_iter->arg_key, 0, sizeof(struct cmsc_String));
  memset(&arg_iter->arg_value, 0, sizeof(struct cmsc_String));

  const char *current_char = arg_iter->buf.buf;
  const char *max_char = arg_iter->buf.buf + arg_iter->buf.len;

  while (current_char != max_char) {
    switch (*current_char) {
    case ';':
      if (!arg_iter->value.buf) {
        return cmsc_arg_iterator_emit_value(arg_iter, current_char, 1);
      }
      if (arg_iter->arg_key.buf) {
        return cmsc_arg_iterator_emit_arg(arg_iter, current_char, 1);
      }
      break;

    case '=':
      if (arg_iter->value.buf && !arg_iter->arg_key.buf) {
        arg_iter->arg_key.buf = arg_iter->buf.buf;
        arg_iter->arg_key.len = (uint32_t)(current_char - arg_iter->buf.buf);
      }
      break;

    case ',':
      if (arg_iter->value.buf) {
        memset(&arg_iter->value, 0, sizeof(struct cmsc_String));
        if (arg_iter->arg_key.buf) {
          return cmsc_arg_iterator_emit_arg(arg_iter, current_char, 1);
        }
      }
      break;

    default:
      break;
    }

    current_char++;
  }

  if (!arg_iter->value.buf) {
    return cmsc_arg_iterator_emit_value(arg_iter, current_char, 0);
  }

  if (arg_iter->arg_key.buf) {
    return cmsc_arg_iterator_emit_arg(arg_iter, current_char, 0);
  }

  return cmsc_ArgNextResults_NONE;
}

#endif

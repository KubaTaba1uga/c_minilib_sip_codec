/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H
#define C_MINILIB_SIP_CODEC_ARGS_ITERATOR_H

#include "parser/iterator/line_iterator.h"

struct cmsc_ArgsIterator {

  struct cmsc_Line arg;
  struct cmsc_Line value;
};

static inline cme_error_t
cmsc_args_iterator_init(const char *buf, uint32_t buf_len,
                        struct cmsc_ArgsIterator *args_iter) {

  cme_error_t err;
  if (!buf || !args_iter || buf_len == 0) {
    err = cme_error(EINVAL, "`buf` and `args_iter` cannot be NULL");
    goto error_out;
  }

  if ((err = cmsc_line_iterator_init(buf, buf_len, &args_iter->line_iter))) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif

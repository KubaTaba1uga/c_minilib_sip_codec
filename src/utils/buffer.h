/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_BUFFER_H
#define C_MINILIB_SIP_CODEC_BUFFER_H

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline cme_error_t cmsc_buffer_insert(const struct cmsc_String value,
                                             struct cmsc_Buffer *buffer,
                                             struct cmsc_String *result) {
  cme_error_t err;

  if (!buffer || !result) {
    err = cme_error(EINVAL, "`buffer` and `result` cannot be NULL");
    goto error_out;
  }

  while (value.len > (buffer->size - buffer->len)) {
    char *buf_cp =
        realloc((void *)buffer->buf, (unsigned int)(buffer->size * 2));
    if (!buf_cp) {
      err = cme_error(ENOMEM, "Cannot allocate memory for new `buffer->buf`");
      goto error_out;
    }

    buffer->buf = buf_cp;
    buffer->size *= 2;
  }

  result->buf = buffer->buf + buffer->len;
  result->len = value.len;
  memcpy((void *)(result->buf), value.buf, value.len);
  buffer->len += value.len;

  return 0;

error_out:
  return cme_return(err);
};

#endif

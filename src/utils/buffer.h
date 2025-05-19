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

  if (!buffer) {
    err = cme_error(EINVAL, "`buffer` cannot be NULL");
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

  if (result) {
    result->buf = buffer->buf + buffer->len;
    result->len = value.len;
  }

  memcpy((void *)(buffer->buf) + buffer->len, value.buf, value.len);
  buffer->len += value.len;

  return 0;

error_out:
  return cme_return(err);
};

#include <stdarg.h>
#include <stdio.h>

static inline cme_error_t cmsc_buffer_finsert(struct cmsc_Buffer *buffer,
                                              struct cmsc_String *result,
                                              const char *fmt, ...) {
  if (!buffer || !fmt)
    return cme_error(EINVAL, "`buffer` and `fmt` must not be NULL");

  va_list args;
  int written;

  // Start with the current buffer length
  char *dst = (char *)(buffer->buf + buffer->len);
  uint32_t space = buffer->size - buffer->len;
  puts("start");
  while (true) {
    va_start(args, fmt);
    written = vsnprintf(dst, space, fmt, args);
    va_end(args);

    printf("Written=%d, space=%d\n", written, space);

    if (written < 0) {
      return cme_error(EINVAL, "Formatting failed");
    }

    // Enough space
    if ((uint32_t)written < space) {
      break;
    }

    // Grow buffer and retry
    uint32_t new_size = buffer->size;
    if (written > new_size) {
      new_size = written;
    }

    new_size *= 2;
    char *new_buf = realloc((void *)buffer->buf, new_size);
    if (!new_buf) {
      return cme_error(ENOMEM, "Cannot realloc buffer");
    }

    printf("New buf: %.*s\n", (int)new_size, new_buf);

    dst = new_buf + buffer->len;
    buffer->buf = new_buf;
    buffer->size = new_size;
    space = buffer->size - buffer->len;
  }

  if (result) {
    result->buf = buffer->buf + buffer->len;
    result->len = (uint32_t)written;
  }

  buffer->len += (uint32_t)written;
  return 0;
}

static inline cme_error_t cmsc_buffer_binsert(const struct cmsc_String value,
                                              struct cmsc_Buffer *buffer,
                                              struct cmsc_BString *result) {
  cme_error_t err;

  if (!buffer) {
    err = cme_error(EINVAL, "`buffer` cannot be NULL");
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

  if (result) {
    result->buf_offset = buffer->len;
    result->len = value.len;
  }

  memcpy((void *)(buffer->buf) + buffer->len, value.buf, value.len);
  buffer->len += value.len;

  return 0;

error_out:
  return cme_return(err);
};

#endif

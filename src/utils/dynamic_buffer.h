/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_DYNAMIC_BUFFER_H
#define C_MINILIB_SIP_CODEC_DYNAMIC_BUFFER_H

#include <stdint.h>

#include <c_minilib_error.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_sip_codec.h"

#include "utils/string.h"

#define CMSC_DYNBUF_GET_END(dynbuf) (dynbuf)->buf + (dynbuf)->len

struct cmsc_DynamicBuffer {
  uint32_t size;
  uint32_t len;
  char buf[];
};

static inline cme_error_t
cmsc_dynbuf_create(uint32_t buf_size, struct cmsc_DynamicBuffer **dynbuf) {
  cme_error_t err;

  if (!dynbuf) {
    err = cme_error(EINVAL, "`dynbuf` cannot be NULL");
    goto error_out;
  }

  struct cmsc_DynamicBuffer *local_dynbuf =
      malloc(sizeof(struct cmsc_DynamicBuffer) + buf_size * sizeof(char));
  if (!local_dynbuf) {
    err = cme_error(ENOMEM, "Unable to allocate memory for `local_dynbuf`");
    goto error_out;
  }

  local_dynbuf->len = 0;
  local_dynbuf->size = buf_size;
  *dynbuf = local_dynbuf;

  return 0;

error_out:
  return cme_return(err);
}

static inline void cmsc_dynbuf_destroy(struct cmsc_DynamicBuffer **dynbuf) {
  if (!dynbuf || !*dynbuf) {
    return;
  }

  free(*dynbuf);
  *dynbuf = NULL;
}

static inline cme_error_t cmsc_dynbuf_put(uint32_t data_len, char *data,
                                          struct cmsc_DynamicBuffer **dynbuf) {

  cme_error_t err;

  if (data_len == 0 || !data) {
    return 0;
  }

  if (!dynbuf || !*dynbuf) {
    err = cme_error(EINVAL, "`dynbuf` cannot be NULL");
    goto error_out;
  }

  struct cmsc_DynamicBuffer *local_dynbuf = *dynbuf;

  uint32_t available_space = local_dynbuf->size - local_dynbuf->len;

  if (data_len > available_space) {
    uint32_t new_size = local_dynbuf->size * 2;
    while ((new_size - local_dynbuf->len) < data_len) {
      new_size *= 2;
    }

    struct cmsc_DynamicBuffer *tmpbuf =
        realloc(local_dynbuf,
                sizeof(struct cmsc_DynamicBuffer) + new_size * sizeof(char));
    if (!tmpbuf) {
      err = cme_error(ENOMEM, "Cannot reallocate memory for `dynbuf`");
      goto error_out;
    }

    local_dynbuf = tmpbuf;
    local_dynbuf->size = new_size;
  }

  memcpy(local_dynbuf->buf + local_dynbuf->len, data, data_len);
  local_dynbuf->len += data_len;

  *dynbuf = local_dynbuf;

  return 0;

error_out:
  return err;
}

static inline char *cmsc_dynbuf_flush(struct cmsc_DynamicBuffer *dynbuf) {
  if (!dynbuf) {
    return NULL;
  }

  while (dynbuf->len < (dynbuf->size / 2)) {
    dynbuf->size = dynbuf->size / 2;
    // We do not need to realloc now,
    // realoc will occur on put and that's fine.
  }

  dynbuf->len = 0;

  return dynbuf->buf;
};

#endif

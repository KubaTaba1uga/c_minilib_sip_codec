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

static inline void *cmsc_dynbuf_malloc(const uint32_t base_size,
                                       const uint32_t elements_amount) {
  return malloc(base_size + (elements_amount * sizeof(char)));
}

static inline cme_error_t cmsc_dynbuf_init(const uint32_t size,
                                           struct cmsc_DynamicBuffer *dynbuf) {
  cme_error_t err;

  if (!dynbuf) {
    err = cme_error(EINVAL, "`dynbuf` cannot be NULL");
    goto error_out;
  }

  dynbuf->len = 0;
  dynbuf->size = size;

  return 0;

error_out:
  return cme_return(err);
}

static inline void cmsc_dynbuf_destroy(struct cmsc_DynamicBuffer *dynbuf) {
  if (!dynbuf) {
    return;
  }

  dynbuf->len = 0;
  dynbuf->size = 0;
}

static inline cme_error_t cmsc_dynbuf_put(uint32_t data_len, char *data,
                                          void **parent_container,
                                          uint32_t parent_container_size,
                                          struct cmsc_DynamicBuffer *dynbuf) {

  cme_error_t err;

  if (data_len == 0 || !data) {
    return 0;
  }

  if (!dynbuf || !parent_container) {
    err = cme_error(EINVAL, "`dynbuf` and `parent_container` cannot be NULL");
    goto error_out;
  }

  uint32_t available_space = dynbuf->size - dynbuf->len;

  if (data_len > available_space) {
    uint32_t new_size = dynbuf->size * 2;
    while ((new_size - dynbuf->len) < data_len) {
      new_size *= 2;
    }

    // Here we are utilizing that dynbuf is embedded in parent
    uintptr_t dynbuf_offset =
        (uintptr_t)dynbuf - (uintptr_t)(*parent_container);

    void *new_parent_container = realloc(
        *parent_container, parent_container_size + new_size * sizeof(char));
    if (!new_parent_container) {
      err = cme_error(ENOMEM, "Cannot reallocate memory for `dynbuf`");
      goto error_out;
    }

    *parent_container = new_parent_container;

    dynbuf = (struct cmsc_DynamicBuffer *)((uintptr_t)new_parent_container +
                                           dynbuf_offset);
    dynbuf->size = new_size;
  }

  memcpy(dynbuf->buf + dynbuf->len, data, data_len);
  dynbuf->len += data_len;

  return 0;

error_out:
  return err;
}

/* Return NULL if fail. Return ptr to valid str on success. */
static inline const char *
cmsc_dynbuf_put_word(uint32_t data_len, char *data, void **parent_container,
                     uint32_t parent_container_size,
                     struct cmsc_DynamicBuffer *dynbuf) {
  char *output = dynbuf->buf + dynbuf->len;
  if (cmsc_dynbuf_put(data_len, data, parent_container, parent_container_size,
                      dynbuf)) {
    return NULL;
  };

  // Ensure value is null terminated
  if (output[data_len - 1] &&
      cmsc_dynbuf_put(1, "", parent_container, parent_container_size, dynbuf)) {
    return NULL;
  };

  return output;
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

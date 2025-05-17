/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_SIPHDR_H
#define C_MINILIB_SIP_CODEC_SIPHDR_H

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline cme_error_t cmsc_siphdr_create(const uint32_t key_len,
                                             const uint32_t key_offset,
                                             const uint32_t value_len,
                                             const uint32_t value_offset,
                                             struct cmsc_SipHeader **hdr) {
  cme_error_t err;

  if (!hdr || !key_len) {
    err = cme_error(EINVAL, "`key` and `hdr` cannot be NULL");
    goto error_out;
  }

  *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  if (!*hdr) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `hdr`");
    goto error_out;
  }

  (*hdr)->key.len = key_len;
  (*hdr)->key.buf_offset = key_offset;
  (*hdr)->value.len = value_len;
  (*hdr)->value.buf_offset = value_offset;

  return 0;

error_out:
  return cme_return(err);
}

#endif

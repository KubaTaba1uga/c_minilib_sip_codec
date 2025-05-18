/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_BSTRING_H
#define C_MINILIB_SIP_CODEC_BSTRING_H

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline struct cmsc_String
cmsc_bs_msg_to_string(const struct cmsc_BString *src,
                      struct cmsc_SipMessage *msg) {
  return (struct cmsc_String){.buf = msg->_buf.buf + src->buf_offset,
                              .len = src->len};
}

static inline struct cmsc_BString
cmsc_s_msg_to_bstring(const struct cmsc_String *src,
                      struct cmsc_SipMessage *msg) {
  return (struct cmsc_BString){.buf_offset = src->buf - msg->_buf.buf,
                               .len = src->len};
}

static inline void cmsc_s_trimm(struct cmsc_String *src, char c_to_sanitize) {
  while (src->len > 0 && *src->buf == c_to_sanitize) {
    src->buf++;
    src->len--;
  }

  while (src->len > 0 && src->buf[src->len - 1] == c_to_sanitize) {
    src->len--;
  }
}

static inline void cmsc_bs_trimm(struct cmsc_BString *src, char c_to_sanitize,
                                 struct cmsc_SipMessage *msg) {
  struct cmsc_String string = {.buf = msg->_buf.buf + src->buf_offset,
                               .len = src->len};
  cmsc_s_trimm(&string, c_to_sanitize);

  src->buf_offset = string.buf - msg->_buf.buf;
  src->len = string.len;
}

#endif

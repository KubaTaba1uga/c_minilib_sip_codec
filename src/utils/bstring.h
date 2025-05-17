/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_BSTRING_H
#define C_MINILIB_SIP_CODEC_BSTRING_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline const char *
cmsc_bstring_dump_ptr(const struct cmsc_BString *bstring,
                      const struct cmsc_Buffer *buf) {
  return buf->buf + bstring->buf_offset;
}

static inline struct cmsc_String
cmsc_bstring_dump_string(const struct cmsc_BString *bstring,
                         const struct cmsc_Buffer *buf) {
  return (struct cmsc_String){.buf = cmsc_bstring_dump_ptr(bstring, buf),
                              .len = bstring->len};
}

#endif

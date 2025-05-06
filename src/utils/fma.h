#ifndef C_MINILIB_SIP_CODEC_FAM_H
#define C_MINILIB_SIP_CODEC_FAM_H

#include <asm-generic/errno.h>
#include <c_minilib_error.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "c_minilib_sip_codec.h"

struct cmsc_FamBuffer {
  uint32_t size;
  uint32_t len;
  char buffer[];
};

static inline cme_error_t
cmsc_fambuffer_init(const uint32_t size, const uint32_t len,
                    struct cmsc_FamBuffer *fambuffer) {
  cme_error_t err;

  if (!fambuffer) {
    err = cme_error(EINVAL, "`fambuffer` cannot be NULL");
    goto error_out;
  }

  fambuffer->len = len;
  fambuffer->size = size;

  return 0;

error_out:
  return cme_return(err);
}

static inline char *
cmsc_fambuffer_insert_str(const uint32_t src_len, const char *src,
                          struct cmsc_FamBuffer *fambuffer) {

  // We are deleting all spaces at front
  while (isblank(*src)) {
    src++;
  }

  // And at tail
  const char *src_end = src + src_len - 1;
  while (isblank(*src_end)) {
    src_end--;
  }
  src_end++;

  char *dest = fambuffer->buffer + fambuffer->len;
  strncpy(dest, src, src_end - src);       // copy src
  fambuffer->len += src_end - src;         // increment len
  fambuffer->buffer[fambuffer->len++] = 0; // insert null for safety

  return dest;
}

#endif // C_MINILIB_SIP_CODEC_FAM_H

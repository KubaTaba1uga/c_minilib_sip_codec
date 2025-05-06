#ifndef C_MINILIB_SIP_CODEC_FAM_H
#define C_MINILIB_SIP_CODEC_FAM_H

#include <asm-generic/errno.h>
#include <c_minilib_error.h>
#include <stdint.h>
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
  uint32_t available_space = fambuffer->size - fambuffer->len;
  if (src_len > available_space) {
    // This shouldn't happn, we specially are picking enough big buffer
    //  when creating fambuffer, but can't be too secure i supouse.
    return NULL;
  }

  char *dest = fambuffer->buffer + fambuffer->len;
  strncpy(dest, src, src_len);             // copy src
  fambuffer->len += src_len;               // increment len
  fambuffer->buffer[fambuffer->len++] = 0; // insert null for safety

  return dest;
}

#endif // C_MINILIB_SIP_CODEC_FAM_H

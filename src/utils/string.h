#ifndef C_MINILIB_SIP_CODEC_STRING_H
#define C_MINILIB_SIP_CODEC_STRING_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

#include <string.h>

static inline const char *cmsc_strnstr(const char *haystack, const char *needle,
                                       size_t haystack_len) {
  size_t needle_len;

  needle_len = strlen(needle);
  if (needle_len > haystack_len) {
    return NULL;
  }

  for (size_t i = 0; i + needle_len <= haystack_len; ++i) {
    if (haystack[i] == needle[0] &&
        memcmp(haystack + i, needle, needle_len) == 0) {
      return haystack + i;
    }
  }
  return NULL;
};

#endif // C_MINILIB_SIP_CODEC_STRING_H

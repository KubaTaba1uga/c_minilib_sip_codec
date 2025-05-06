#ifndef C_MINILIB_SIP_CODEC_COMMON_PARSE_H
#define C_MINILIB_SIP_CODEC_COMMON_PARSE_H

#include <c_minilib_error.h>
#include <stdint.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "utils/fma.h"
#include "utils/string.h"

static inline const char *cmsc_parse_tag_field(const uint32_t buffer_len,
                                               const char *buffer,
                                               struct cmsc_FamBuffer *fambuffer,
                                               char **out, uint32_t field_flag,
                                               cmsc_sipmsg_t msg) {
  const char *tag_start = cmsc_strnstr(buffer, "tag", buffer_len);
  if (tag_start) {
    *out = cmsc_fambuffer_insert_str(buffer_len - (tag_start - buffer),
                                     tag_start, fambuffer);
    cmsc_message_mark_field_present(msg, field_flag);
  }
  return tag_start;
}

static inline const char *
cmsc_parse_display_name(const uint32_t buffer_len, const char *buffer,
                        struct cmsc_FamBuffer *fambuffer, char **out,
                        uint32_t field_flag, cmsc_sipmsg_t msg) {
  /* According RFC 3261 20.10: Contact
       When the header field value contains a display name, the URI
       including all URI parameters is enclosed in "<" and ">".
  */  
  const char *display_name_end = cmsc_strnstr(buffer, "<", buffer_len);
  if (display_name_end && display_name_end != buffer) {
    *out =
        cmsc_fambuffer_insert_str(display_name_end - buffer, buffer, fambuffer);
    cmsc_message_mark_field_present(msg, field_flag);
  }
  return display_name_end;
}

static inline void
cmsc_parse_uri_field(const char *buffer, const char *display_name_end,
                     const char *tag_start, struct cmsc_FamBuffer *fambuffer,
                     char **out, uint32_t field_flag, cmsc_sipmsg_t msg) {
  const char *value_start = (display_name_end) ? display_name_end : buffer;
  const char *value_end = (tag_start) ? tag_start : buffer + strlen(buffer);

  if (*(value_end - 1) == ';') {
    value_end--;
  }

  *out = cmsc_fambuffer_insert_str(value_end - value_start, value_start,
                                   fambuffer);
  cmsc_message_mark_field_present(msg, field_flag);
}

#endif

#ifndef C_MINILIB_SIP_CODEC_PARSE_H
#define C_MINILIB_SIP_CODEC_PARSE_H

#include <c_minilib_error.h>
#include <stdint.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "utils/fma.h"
#include "utils/string.h"

static inline cme_error_t cmsc_parse_field_func_to(const uint32_t buffer_len,
                                                   const char *buffer,
                                                   cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.2 To and 20.39 To this field
     need to parse two values:
        - SIP uri
        - tag (optional)
        - display name (optional)
      We cannot know sip uri scheme user will be using so SIP uri
      is copied as is. No uri scheme interpretation.
   */

  const char *tag_start = cmsc_strnstr(buffer, "tag", buffer_len);
  if (tag_start) {
    msg->to.tag = cmsc_fambuffer_insert_str(buffer_len - (tag_start - buffer),
                                            tag_start, &msg->_buffer);
    cmsc_message_mark_field_present(msg, cmsc_SipField_TO_TAG);
  }

  /* According RFC 3261 20.10: Contact
       When the header field value contains a display name, the URI
       including all URI parameters is enclosed in "<" and ">".
  */
  const char *display_name_end = cmsc_strnstr(buffer, "<", buffer_len);
  if (display_name_end && display_name_end != buffer) {
    msg->to.display_name = cmsc_fambuffer_insert_str(
        (display_name_end - buffer), buffer, &msg->_buffer);
    cmsc_message_mark_field_present(msg, cmsc_SipField_TO_DISPLAY_NAME);
  }

  const char *value_start = buffer;
  if (display_name_end) {
    value_start = display_name_end;
  }

  const char *value_end = buffer + buffer_len;
  if (tag_start) {
    value_end = tag_start;
  }

  if (*(value_end - 1) == ';') {
    value_end -= 1;
  }

  msg->to.uri = cmsc_fambuffer_insert_str(value_end - value_start, value_start,
                                          &msg->_buffer);
  cmsc_message_mark_field_present(msg, cmsc_SipField_TO_URI);

  return 0;
}

#endif // C_MINILIB_SIP_CODEC_PARSE_H

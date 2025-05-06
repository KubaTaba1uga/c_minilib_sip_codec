#ifndef C_MINILIB_SIP_CODEC_PARSE_H
#define C_MINILIB_SIP_CODEC_PARSE_H

#include <c_minilib_error.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "utils/fma.h"
#include "utils/string.h"

static inline cme_error_t cmsc_parse_field_func_to(const uint32_t buffer_len,
                                                   const char *buffer,
                                                   cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.2 To and 20.39 To this field
     need to parse two values:
        - SIP uri
        - tag (optional)
      We cannot know sip uri scheme user will be using so SIP uri
      is copied as is. No uri scheme interpretation.
   */

  const char *tag_start = cmsc_strnstr(buffer, "tag", buffer_len);
  if (tag_start) {
    msg->to.tag = cmsc_fambuffer_insert_str(buffer_len - (tag_start - buffer),
                                            tag_start, &msg->_buffer);
    msg->to.uri =
        cmsc_fambuffer_insert_str(tag_start - buffer, buffer, &msg->_buffer);
  } else {
    msg->to.uri = NULL;
    msg->to.tag = cmsc_fambuffer_insert_str(buffer_len, buffer, &msg->_buffer);
  }

  return 0;
}

#endif // C_MINILIB_SIP_CODEC_PARSE_H

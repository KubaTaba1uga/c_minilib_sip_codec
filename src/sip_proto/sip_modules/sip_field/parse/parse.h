#ifndef C_MINILIB_SIP_CODEC_PARSE_H
#define C_MINILIB_SIP_CODEC_PARSE_H

#include <asm-generic/errno-base.h>
#include <c_minilib_error.h>
#include <stdint.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "sip_proto/sip_modules/sip_field/parse/common_parse.h"
#include "utils/fma.h"
#include "utils/string.h"

static inline cme_error_t cmsc_parse_field_func_to(const uint32_t buffer_len,
                                                   const char *buffer,
                                                   cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.2 To and 20.39 To this field
     need to parse three values:
        - SIP uri
        - tag (optional)
        - display name (optional)
      We cannot know sip uri scheme user will be using so SIP uri
      is copied as is. No uri scheme interpretation.
   */

  const char *tag_start = cmsc_parse_tag_field(
      buffer_len, buffer, cmsc_SipField_TO_TAG, (char **)&msg->to.tag, msg);

  const char *display_name_end =
      cmsc_parse_display_name(buffer_len, buffer, cmsc_SipField_TO_DISPLAY_NAME,
                              (char **)&msg->to.display_name, msg);

  cmsc_parse_uri_field(buffer_len, buffer, display_name_end, tag_start,
                       cmsc_SipField_TO_URI, (char **)&msg->to.uri, msg);

  return 0;
}

static inline cme_error_t cmsc_parse_field_func_from(const uint32_t buffer_len,
                                                     const char *buffer,
                                                     cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.3 From and 20.20 From, this field
     need to parse three values:
        - SIP uri
        - tag (optional)
        - display name (optional)
      We cannot know sip uri scheme user will be using so SIP uri
      is copied as is. No uri scheme interpretation.
   */

  const char *tag_start = cmsc_parse_tag_field(
      buffer_len, buffer, cmsc_SipField_FROM_TAG, (char **)&msg->from.tag, msg);

  const char *display_name_end = cmsc_parse_display_name(
      buffer_len, buffer, cmsc_SipField_FROM_DISPLAY_NAME,
      (char **)&msg->from.display_name, msg);

  cmsc_parse_uri_field(buffer_len, buffer, display_name_end, tag_start,
                       cmsc_SipField_FROM_URI, (char **)&msg->from.uri, msg);
  return 0;
}

static inline cme_error_t cmsc_parse_field_func_cseq(const uint32_t buffer_len,
                                                     const char *buffer,
                                                     cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.5 CSeq, this field
     need to parse one value:
        - cseq
     This field also requires validation:
        The method MUST match that of the request.
   */

  cme_error_t err;
  if (msg->is_request &&
      !cmsc_strnstr(buffer, cmsc_dump_sip_method_to_str(msg->sip_method),
                    buffer_len)) {
    err = cme_errorf(EINVAL, "CSeq value does not match request method: %.*s",
                     buffer_len, buffer);
    goto error_out;
  }

  msg->cseq = cmsc_fambuffer_insert_str(buffer_len, buffer, &msg->_buffer);
  cmsc_message_mark_field_present(msg, cmsc_SipField_CSEQ);

  return 0;

error_out:
  return err;
}

static inline cme_error_t
cmsc_parse_field_func_callid(const uint32_t buffer_len, const char *buffer,
                             cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.4 Call-id, this field
     need to parse one value:
        - call id
   */

  msg->call_id = cmsc_fambuffer_insert_str(buffer_len, buffer, &msg->_buffer);
  cmsc_message_mark_field_present(msg, cmsc_SipField_CALL_ID);

  return 0;
}

static inline cme_error_t
cmsc_parse_field_func_max_forwards(const uint32_t buffer_len,
                                   const char *buffer, cmsc_sipmsg_t msg) {
  /* According RFC 3261 8.1.1.6 Max-Forwards, this field
     need to parse one value:
        - max forwards
   */

  msg->max_forwards = atoi(buffer);
  cmsc_message_mark_field_present(msg, cmsc_SipField_MAX_FORWARDS);

  return 0;
}

#endif // C_MINILIB_SIP_CODEC_PARSE_H

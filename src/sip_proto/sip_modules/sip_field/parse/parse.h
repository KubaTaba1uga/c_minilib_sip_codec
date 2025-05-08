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

#define CMSC_GET_BUFFER_END(buffer, buffer_len) ((buffer) + (buffer_len)-1)

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

static inline cme_error_t cmsc_parse_field_func_via(const uint32_t buffer_len,
                                                    const char *buffer,
                                                    cmsc_sipmsg_t msg) {
  /* According RFC 3261 25, this field
     need to parse one value, but on different ways:
       - One Via header with one via-parm
       - One Via header with multiple via-parm, comma-separated
       - Multiple Via headers, each with a single or multiple via-parm

     Additionally this field needs to populate these optional attributes:
       - ttl
       - addr
       - received
       - branch
       - extension
   */
  const char *buffer_end = CMSC_GET_BUFFER_END(buffer, buffer_len);
  const char *value_start;
  const char *value_end;
  cme_error_t err;

  // Validate input
  if (!(value_start = cmsc_strnstr(buffer, "SIP/2.0/", buffer_len))) {
    err =
        cme_errorf(EINVAL, "Via field value has to start from `SIP/2.0`: %.*s",
                   buffer_len, buffer);
    goto error_out;
  }

  uint32_t buffer_len_ = buffer_len;

  value_start += 8; // len("SIP/2.0/") = 8
  buffer_len_ -= 8;

  const char *next_start = NULL;
  if ((value_end = cmsc_strnstr(value_start, ",", buffer_len_))) {
    next_start = value_end;
  } else {
    value_end = CMSC_GET_BUFFER_END(buffer, buffer_len_);
  }

  // Construct list
  struct cmsc_SipVia *via = &msg->via_l;
  if (cmsc_message_is_field_present(msg, cmsc_SipField_VIA_L)) {
    if (via->next) {
      while (via->next) {
        via = via->next;
      }
    }

    via->next = calloc(1, sizeof(struct cmsc_SipVia));
    if (!via->next) {
      err = cme_error(ENOMEM, "Unable to allocate memory for `via->next`");
      goto error_out;
    }

    via = via->next;
  }

  // Write values
  uint32_t written_bytes;
  via->transp_proto = cmsc_parse_transport_proto(value_end - value_start,
                                                 value_start, &written_bytes);

  if (!via->transp_proto) {
    err = cme_errorf(
        EINVAL, "Via field value has to contain transport protocol info: %.*s",
        buffer_len_, buffer);
    goto error_next_cleanup;
  }

  if (!(value_start =
            cmsc_strnstr(value_start, " ", value_end - value_start))) {
    err = cme_errorf(EINVAL, "Via field value has invalid format: %.*s",
                     buffer_len_, buffer);
    goto error_out;
  }

  {
    const char *value_end_ = value_end;
    if ((value_end_ =
             cmsc_strnstr(value_start, ";", value_end - value_start))) {

      value_end_--;
      value_end = value_end_;

      const char *branch =
          cmsc_strnstr(value_end, "branch=", buffer_end - value_end);
      if (branch) {
        via->branch = cmsc_parse_insert_sip_arg(
            (buffer + buffer_len - 1) - value_end, branch, msg);
      }

      const char *addr =
          cmsc_strnstr(value_end, "addr=", buffer_end - value_end);
      if (addr) {
        via->addr = cmsc_parse_insert_sip_arg(
            (buffer + buffer_len - 1) - value_end, addr, msg);
      }

      const char *received =
          cmsc_strnstr(value_end, "received=", buffer_end - value_end);
      if (received) {
        via->received =
            cmsc_parse_insert_sip_arg(buffer_end - value_end, received, msg);
      }

      const char *ttl = cmsc_strnstr(value_end, "ttl=", buffer_end - value_end);
      if (ttl) {
        ttl += 4; // len("ttl=") is 4
        via->ttl = atoi(ttl);
      }
    }
  }

  via->sent_by = cmsc_fambuffer_insert_str(value_end - value_start, value_start,
                                           &msg->_buffer);
  cmsc_message_mark_field_present(msg, cmsc_SipField_VIA_L);

  // Find next values
  if (next_start) {
    err = cmsc_parse_field_func_via(
        CMSC_GET_BUFFER_END(buffer, buffer_len) - next_start, next_start, msg);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_next_cleanup:
  if (via != &msg->via_l) {
    free(via);
  }

error_out:
  return cme_return(err);
}

#endif // C_MINILIB_SIP_CODEC_PARSE_H

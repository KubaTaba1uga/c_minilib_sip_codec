#ifndef C_MINILIB_SIP_CODEC_COMMON_SIP_PROTO_H
#define C_MINILIB_SIP_CODEC_COMMON_SIP_PROTO_H

#include <ctype.h>
#include <stdint.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"

#define CMSC_SCHEME_MANDATORY_FIELDS_ITER(var, scheme, ...)                    \
  for (uint32_t __i__ = 0; __i__ < (scheme)->mandatory.len; ++__i__) {         \
    struct cmsc_SchemeField var = (scheme)->mandatory.fields[__i__];           \
    __VA_ARGS__;                                                               \
  }

/* Use schemes to parse buffer. Goal of this function is to set
   msg->sip_msg_type. Once we have msg_type we can fetch appropriate scheme
   on demand.
*/
static inline cme_error_t cmsc_sip_proto_parse_first_line(
    const uint32_t schemes_len, const struct cmsc_Scheme **schemes,
    const uint32_t buffer_len, const char *buffer, cmsc_sipmsg_t msg) {
  const char *first_line_end;
  cme_error_t err;

  if (!(first_line_end = strstr(buffer, "\r\n"))) {
    err = cme_errorf(
        EINVAL, "No CLRF indicating first line in the message: %.64s", buffer);
    goto error_out;
  }

  const struct cmsc_Scheme *scheme = NULL;
  const char *msg_type_match;

  for (uint32_t i = 0; i < schemes_len; i++) {
    const struct cmsc_Scheme *local_scheme = schemes[i];
    msg_type_match = strstr(buffer, local_scheme->sip_msg_type_str);
    if (msg_type_match && msg_type_match < first_line_end) {
      scheme = local_scheme;
      break;
    }
  }

  if (!scheme) {
    err = cme_errorf(
        ENOENT, "Cannot find supported scheme for the message: %.64s", buffer);
    goto error_out;
  }

  const char *sip_version_match = NULL;
  sip_version_match = strstr(buffer, "SIP");
  if (!sip_version_match || sip_version_match > first_line_end) {
    err = cme_error(ENOENT, "Cannot find SIP version in msg first line");
    goto error_out;
  }

  // Skip `SIP/`
  sip_version_match += 4;

  if (sip_version_match > first_line_end || !isdigit(*sip_version_match)) {
    err = cme_errorf(ENOENT, "SIP major version has invalid format: %s",
                     sip_version_match);
    goto error_out;
  }

  char major_buf[2];
  major_buf[0] = *sip_version_match;
  major_buf[1] = 0;
  // TO-DO mark this as present
  msg->sip_proto_ver.major = atoi(major_buf);

  // Skip `2.`
  sip_version_match += 2;

  if (sip_version_match > first_line_end || !isdigit(*sip_version_match)) {
    err = cme_errorf(ENOENT, "SIP minor version has invalid format: %s",
                     sip_version_match);
    goto error_out;
  }

  char minor_buf[2];
  minor_buf[0] = *sip_version_match;
  minor_buf[1] = 0;
  msg->sip_proto_ver.minor = atoi(minor_buf);
  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_PROTO_VER);

  msg->is_request = sip_version_match > msg_type_match;
  cmsc_message_mark_field_present(msg, cmsc_SipField_IS_REQUEST);

  msg->sip_msg_type = scheme->sip_msg_type;
  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_MSG_TYPE);

  msg->sip_method = cmsc_map_sip_msg_type_to_sip_method(msg->sip_msg_type);
  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_METHOD);

  return 0;

error_out:
  return cme_return(err);
};

/* Default fallback for recognising key in header field. */
static inline bool cmsc_default_is_field_func(const uint32_t buffer_len,
                                              const char *buffer,
                                              const char *id) {

  uint32_t id_len = strlen(id);
  if (id_len > buffer_len) {
    return false;
  }

  id_len++;

  char tmp_buffer[id_len];
  char tmp_id[id_len];
  strncpy(tmp_buffer, buffer, id_len);
  tmp_buffer[id_len - 1] = 0;
  strncpy(tmp_id, id, id_len);
  tmp_id[id_len - 1] = 0;

  // According to RFC 3261 7.3.1 case doesn't matter.
  for (uint32_t i = 0; i < id_len; i++) {
    tmp_buffer[i] = tolower(tmp_buffer[i]);
    tmp_id[i] = tolower(tmp_id[i]);
  }

  return strcmp(tmp_id, tmp_buffer) == 0;
};

#endif // C_MINILIB_SIP_CODEC_COMMON_SIP_PROTO_H

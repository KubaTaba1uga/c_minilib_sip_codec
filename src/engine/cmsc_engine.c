#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "engine/cmsc_engine.h"
#include "engine/message_type/cmsc_invite.h"

#define CMSC_SCHEMES_ITER(var, func)                                           \
  for (int i__ = 0; i__ < sizeof(cmsc_schemes) / sizeof(struct cmsc_Scheme *); \
       i__++) {                                                                \
    struct cmsc_Scheme *var = cmsc_schemes[i__];                               \
    if (!var) {                                                                \
      continue;                                                                \
    }                                                                          \
    func                                                                       \
  }

static struct cmsc_Scheme *cmsc_schemes[] = {
    [cmsc_SipMsgType_INVITE] = NULL,
    // Add more message types here
};

cme_error_t cmsc_engine_init(void) {
  cme_error_t err;

  // We need to initialize possible holes between ptrs.
  memset(cmsc_schemes, 0, sizeof(cmsc_schemes) / sizeof(struct cmsc_Scheme *));

  if (!cmsc_schemes[cmsc_SipMsgType_INVITE] &&
      (err = cmsc_engine_invite_scheme_init(
           &cmsc_schemes[cmsc_SipMsgType_INVITE]))) {
    goto error_out;
  }

  return 0;

error_out:
  cmsc_engine_destroy();
  return cme_return(err);
};

void cmsc_engine_destroy(void) {
  if (cmsc_schemes[cmsc_SipMsgType_INVITE]) {
    cmsc_engine_invite_scheme_destroy(&cmsc_schemes[cmsc_SipMsgType_INVITE]);
    cmsc_schemes[cmsc_SipMsgType_INVITE] = NULL;
  }
};

cme_error_t cmsc_engine_parse(const uint32_t n, const char *buffer,
                              cmsc_sipmsg_t msg) {
  /* Parsing goes sth like this:
    0. Allocate sip message
    1. Parse request/response line
    2. Find appropriate scheme based on step 1 result
    3. For each mandatory field parse buffer
    4. For each optional field parse buffer
    5. Return parsed message
  */
  cme_error_t err;

  if ((err = cmsc_engine_parse_first_line(n, buffer, msg))) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_engine_parse_first_line(const uint32_t n, const char *buffer,
                                         cmsc_sipmsg_t msg) {
  const char *first_line_end;
  cme_error_t err;

  if (!(first_line_end = strstr(buffer, "\r\n"))) {
    err = cme_error(EINVAL, "No CLRF indicating first line");
    goto error_out;
  }

  struct cmsc_Scheme *scheme = NULL;
  const char *msg_type_match;
  CMSC_SCHEMES_ITER(local_scheme, {
    msg_type_match = strstr(buffer, local_scheme->sip_msg_type_str);
    if (msg_type_match && msg_type_match < first_line_end) {
      scheme = local_scheme;
      break;
    }
  })

  if (!scheme) {
    err = cme_error(ENOENT, "Cannot find supported scheme for the message");
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
  // TO-DO mark this as present
  msg->sip_proto_ver.minor = atoi(minor_buf);

  // TO-DO mark this as present
  msg->is_request = sip_version_match > msg_type_match;
  // TO-DO mark this as present
  msg->sip_msg_type = scheme->sip_msg_type;

  return 0;

error_out:
  return cme_return(err);
};

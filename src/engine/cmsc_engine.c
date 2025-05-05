#include <asm-generic/errno-base.h>
#include <c_minilib_error.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "engine/cmsc_engine.h"
#include "engine/message_type/cmsc_invite.h"

static struct cmsc_Scheme *cmsc_schemes[] = {
    [cmsc_SipMsgType_INVITE] = NULL,
    // Add more message types here
};

cme_error_t cmsc_engine_init(void) {
  cme_error_t err;

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

  struct cmsc_Scheme *scheme = NULL;
  const char *first_line_end = strstr(buffer, "\r\n");
  if (!first_line_end) {
    err = cme_error(EINVAL, "No CLRF indicating first line");
    goto error_out;
  }

  for (int i = cmsc_SipMsgType_NONE + 1; i < cmsc_SipMsgType_MAX; i++) {
    const char *match = strstr(buffer, cmsc_schemes[i]->sip_msg_type_str);
    if (match && match < first_line_end) {
      scheme = cmsc_schemes[i];

      break;
    }
  }

  if (!scheme) {
    err = cme_error(ENOENT, "Cannot find supported scheme for the message");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_engine_parse_first_line(const uint32_t n, const char *buffer,
                                         cmsc_sipmsg_t msg) {
  return 0;
};

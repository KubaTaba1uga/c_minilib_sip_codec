#include <asm-generic/errno-base.h>
#include <c_minilib_error.h>
#include <stdint.h>
#include <stdlib.h>

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
  return 0;
};

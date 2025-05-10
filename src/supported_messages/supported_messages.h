#ifndef C_MINILIB_SIP_CODEC_SUPPORTED_MESSAGES_H
#define C_MINILIB_SIP_CODEC_SUPPORTED_MESSAGES_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "supported_messages/invite.h"

static inline cme_error_t cmsc_supmsg_init(void) {
  cme_error_t err;

  if ((err = cmsc_supmsg_invite_init())) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline void cmsc_supmsg_destroy(void) { cmsc_supmsg_invite_destroy(); };

#endif // C_MINILIB_SIP_CODEC_SUPPORTED_MESSAGES_H

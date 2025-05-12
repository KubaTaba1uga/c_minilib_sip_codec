#ifndef C_MINILIB_SIP_CODEC_INVITE_H
#define C_MINILIB_SIP_CODEC_INVITE_H
#include <c_minilib_error.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "scheme/scheme_register.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/common_supported_messages.h"

/* Invite is composed of:
     - Allow (done)
     - Accept
     - Expires
     - Subject
     - Organization
     - User-Agent
     - Body
*/
static inline cme_error_t cmsc_supmsg_invite_init(void) {
  struct cmsc_Scheme invite_scheme;
  cme_error_t err;

  if ((err = cmsc_scheme_init(cmsc_SupportedMessages_INVITE, &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_to, &invite_scheme))) {
    goto error_out;
  };

  /* if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_from,
   * local_scheme)))
   * { */
  /*   goto error_out; */
  /* }; */

  /* if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_cseq,
   * local_scheme)))
   * { */
  /*   goto error_out; */
  /* }; */

  /* if ((err = */
  /*          cmsc_scheme_add_mandatory_field(&cmsc_field_callid,
   * local_scheme)))
   * { */
  /*   goto error_out; */
  /* }; */

  /* if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_max_forwards, */
  /*                                            local_scheme))) { */
  /*   goto error_out; */
  /* }; */

  /* *scheme = local_scheme; */

  if ((err = cmsc_schemes_register_add_scheme(invite_scheme))) {
    goto error_out;
  }

  return 0;

error_out:
  cmsc_scheme_destroy(&invite_scheme);
  return cme_return(err);
};

static inline void cmsc_supmsg_invite_destroy(void) {
  struct cmsc_Scheme *invite_scheme;

  invite_scheme =
      cmsc_schemes_register_get_scheme(cmsc_SupportedMessages_INVITE);
  if (!invite_scheme) {
    return;
  }

  cmsc_scheme_destroy(invite_scheme);
};

#endif // C_MINILIB_SIP_CODEC_INVITE_H

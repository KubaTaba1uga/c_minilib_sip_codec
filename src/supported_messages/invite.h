#ifndef C_MINILIB_SIP_CODEC_INVITE_H
#define C_MINILIB_SIP_CODEC_INVITE_H
#include <c_minilib_error.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "scheme/scheme_register.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/common_supported_messages.h"

static inline cme_error_t cmsc_supmsg_invite_init(void) {
  struct cmsc_Scheme invite_scheme;
  cme_error_t err;

  if ((err = cmsc_scheme_init(cmsc_SupportedMessages_INVITE, &invite_scheme))) {
    goto error_out;
  }

  // --- Mandatory Fields ---
  if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_to, &invite_scheme))) {
    goto error_out;
  }

  if ((err =
           cmsc_scheme_add_mandatory_field(&cmsc_field_from, &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_call_id,
                                             &invite_scheme))) {
    goto error_out;
  }

  if ((err =
           cmsc_scheme_add_mandatory_field(&cmsc_field_cseq, &invite_scheme))) {
    goto error_out;
  }

  if ((err =
           cmsc_scheme_add_mandatory_field(&cmsc_field_via, &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_mandatory_field(&cmsc_field_max_forwards,
                                             &invite_scheme))) {
    goto error_out;
  }

  // --- Optional Fields ---
  if ((err = cmsc_scheme_add_optional_field(&cmsc_field_subject,
                                            &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_optional_field(&cmsc_field_user_agent,
                                            &invite_scheme))) {
    goto error_out;
  }

  if ((err =
           cmsc_scheme_add_optional_field(&cmsc_field_allow, &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_optional_field(&cmsc_field_organization,
                                            &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_optional_field(&cmsc_field_accept,
                                            &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_scheme_add_optional_field(&cmsc_field_expires,
                                            &invite_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_schemes_register_add_scheme(invite_scheme))) {
    goto error_out;
  }

  return 0;

error_out:
  puts(err->msg);
  cmsc_scheme_destroy(&invite_scheme);
  return cme_return(err);
}

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

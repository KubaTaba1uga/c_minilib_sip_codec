#ifndef C_MINILIB_SIP_CODEC_OK_200_H
#define C_MINILIB_SIP_CODEC_OK_200_H
#include <c_minilib_error.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "scheme/scheme_register.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/common_supported_messages.h"

static inline cme_error_t cmsc_supmsg_ok_200_init(void) {
  struct cmsc_Scheme ok_200_scheme;
  cme_error_t err;

  if ((err = cmsc_scheme_init(cmsc_SupportedMessages_200_OK, &ok_200_scheme))) {
    goto error_out;
  }

  if ((err = cmsc_schemes_register_add_scheme(ok_200_scheme))) {
    goto error_out;
  }

  return 0;

error_out:
  cmsc_scheme_destroy(&ok_200_scheme);
  return cme_return(err);
};

static inline void cmsc_supmsg_ok_200_destroy(void) {
  struct cmsc_Scheme *ok_200_scheme;

  ok_200_scheme =
      cmsc_schemes_register_get_scheme(cmsc_SupportedMessages_200_OK);
  if (!ok_200_scheme) {
    return;
  }

  cmsc_scheme_destroy(ok_200_scheme);
};

#endif // C_MINILIB_SIP_CODEC_OK_200_H

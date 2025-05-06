#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "sip_proto/sip_modules/invite.h"

cme_error_t cmsc_sip_proto_invite_init(struct cmsc_Scheme **scheme) {
  struct cmsc_Scheme *local_scheme;
  cme_error_t err;

  if ((err = cmsc_scheme_create(cmsc_SipMsgType_INVITE, "INVITE",
                                &local_scheme))) {
    goto error_out;
  }

  *scheme = local_scheme;

  return 0;

error_out:
  return err;
};
void cmsc_sip_proto_invite_destroy(struct cmsc_Scheme **scheme) {
  cmsc_scheme_destroy(scheme);
};

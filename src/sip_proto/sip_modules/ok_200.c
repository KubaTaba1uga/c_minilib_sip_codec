#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "sip_proto/sip_modules/ok_200.h"

cme_error_t cmsc_sip_proto_ok_200_init(struct cmsc_Scheme **scheme) {
  struct cmsc_Scheme *local_scheme;
  cme_error_t err;

  if ((err = cmsc_scheme_create(cmsc_SipMsgType_200_OK, "200 OK",
                                &local_scheme))) {
    goto error_out;
  }

  *scheme = local_scheme;

  return 0;

error_out:
  return err;
};

void cmsc_sip_proto_ok_200_destroy(struct cmsc_Scheme **scheme) {
  cmsc_scheme_destroy(scheme);
};

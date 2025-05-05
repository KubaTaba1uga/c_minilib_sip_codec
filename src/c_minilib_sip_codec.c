#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "engine/cmsc_engine.h"
#include "scheme/cmsc_scheme.h"

cme_error_t cmsc_init(void) {
  cme_error_t err;

  if (cme_init() != 0) {
    err = cme_error(ENOMEM, "Unable to initiate c_minilib_error library");
    goto error_out;
  };

  if ((err = cmsc_engine_init())) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

void cmsc_destroy(void) {
  cmsc_engine_destroy();

  cme_destroy();
};

cme_error_t cmsc_parse_sip_msg(const uint32_t n, const char *buffer,
                               cmsc_sipmsg_t *msg) {
  return 0;
}

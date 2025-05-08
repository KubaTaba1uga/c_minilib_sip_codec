#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "settings/settings.h"

cme_error_t cmsc_init(void) {
  cme_error_t err;

  cmsc_log(cmsc_LogLevels_DEBUG, "Initializing C minlib sip codec library");

  if (cme_init() != 0) {
    err = cme_error(ENOMEM, "Unable to initiate c_minilib_error library");
    goto error_out;
  };

  cmsc_log(cmsc_LogLevels_DEBUG, "C minlib sip codec lib initialized");

  return 0;

error_out:
  return cme_return(err);
};

void cmsc_destroy(void) { cme_destroy(); };

#include <asm-generic/errno-base.h>
#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "scheme/cmsc_scheme.h"

cme_error_t cmsc_init(void) {
  if (cme_init() != 0) {
    return cme_error(ENOMEM, "Unable to initiate c_minilib_error library");
  };

  cme_error_t err;

  if ((err = cmsc_scheme_init())) {
    return err;
  }

  return 0;
};

void cmsc_destroy(void) {
  cmsc_scheme_destroy();

  cme_destroy();
};

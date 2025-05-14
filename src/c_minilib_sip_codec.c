#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"

cme_error_t cmsc_init(void) {
  cme_error_t err;

  if (cme_init() != 0) {
    err = cme_error(ENOMEM, "Unable to initiate c_minilib_error library");
    goto error_out;
  };

  return 0;

error_out:
  return cme_return(err);
};

void cmsc_destroy(void) { cme_destroy(); };

cme_error_t cmsc_parse_sip(uint32_t buf_len, const char *buf,
                           struct cmsc_SipMessage **msg) {
  cme_error_t err;
  if (!buf || !msg) {
    err = cme_error(EINVAL, "`buf` and `msg` cannot be NULL");
    goto error_out;
  }

  err = cmsc_sipmsg_create(
      (struct cmsc_Buffer){.buf = buf, .len = buf_len, .size = buf_len}, msg);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

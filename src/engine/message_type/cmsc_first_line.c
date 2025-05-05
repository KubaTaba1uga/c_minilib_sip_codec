#include <c_minilib_error.h>
#include <errno.h>

#include "c_minilib_sip_codec.h"
#include "engine/message_type/cmsc_first_line.h"

cme_error_t cmsc_first_line_parse(const uint32_t buffer_len, const char *buffer,
                                  const cmsc_sipmsg_t msg) {
  cme_error_t err;
  if (!buffer || !msg) {
    err = cme_error(EINVAL, "`buffer` and `msg` cannot be NULL");
    goto error_out;
  }

  return 0;

error_out:
  return err;
};

#include <asm-generic/errno-base.h>
#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "engine/message_type/cmsc_message_type.h"

cme_error_t cmsc_message_type_parse(const uint32_t buffer_len,
                                    const char *buffer,
                                    enum cmsc_SipMsgType *msg_type) {
  cme_error_t err;
  if (!buffer || !msg_type) {
    err = cme_error(EINVAL, "`buffer` and `msg_type` cannot be NULL");
    goto error_out;
  }

  
  
  return 0;

error_out:
  return err;
};

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "utils/fma.h"

cme_error_t cmsc_message_create(struct cmsc_SipMessage **message,
                                uint32_t buffer_size) {
  cme_error_t err;
  if (!message) {
    err = cme_error(EINVAL, "`message` cannot be NULL");
    goto error_out;
  }

  // All fields are initialized to 0
  *message = calloc(1, sizeof(struct cmsc_SipMessage) + buffer_size);
  if (!*message) {
    err = cme_error(ENOMEM, "Unable to allocate memory for `message`");
    goto error_out;
  }

  if ((err = cmsc_fambuffer_init(buffer_size, 0, &(*message)->_buffer))) {
    goto error_out;
  };

  return NULL;

error_out:
  return cme_return(err);
}

void cmsc_message_destroy(struct cmsc_SipMessage **message) {
  if (!message || !*message) {
    return;
  }

  free(*message);
  *message = NULL;
}

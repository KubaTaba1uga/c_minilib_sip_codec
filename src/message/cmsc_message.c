#include <stdlib.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "message/cmsc_message.h"

cme_error_t cmsc_message_create(struct cmsc_SipMessage **message) {
  cme_error_t err;
  if (!message) {
    err = cme_error(EINVAL, "`message` cannot be NULL");
    goto error_out;
  }

  // All fields are initialized to 0
  *message = calloc(1, sizeof(struct cmsc_SipMessage));
  if (!*message) {
    err = cme_error(ENOMEM, "Unable to allocate memory for `message`");
    goto error_out;
  }

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

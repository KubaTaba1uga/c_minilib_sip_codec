#include "c_minilib_error.h"
#include "scheme/scheme.h"
#include "sipmsg/sipmsg.h"
#include <stdint.h>
#include <string.h>

struct cmsc_SchemeRegister {
  struct cmsc_Scheme schemes[cmsc_SupportedMessages_MAX];
};

static struct cmsc_SchemeRegister scheme_register;

cme_error_t cmsc_schemes_register_init(void) {
  memset(&scheme_register.schemes, 0, sizeof(struct cmsc_SchemeRegister));

  return NULL;
};
void cmsc_schemes_register_destroy(void);

cme_error_t cmsc_schemes_register_add_scheme(struct cmsc_Scheme scheme) {
  cme_error_t err;

  if (scheme.supmsg <= cmsc_SupportedMessages_NONE ||
      scheme.supmsg >= cmsc_SupportedMessages_MAX) {
    err = cme_error(EINVAL, "Invalid `scheme.supmsg`");
    goto error_out;
  }

  scheme_register.schemes[scheme.supmsg] = scheme;

  return 0;

error_out:
  return cme_return(err);
};

struct cmsc_Scheme *
cmsc_schemes_register_get_scheme(enum cmsc_SupportedMessages supmsg) {
  if (supmsg <= cmsc_SupportedMessages_NONE ||
      supmsg >= cmsc_SupportedMessages_MAX) {
    return NULL;
  }

  struct cmsc_Scheme *scheme = &scheme_register.schemes[supmsg];

  if (scheme->supmsg == cmsc_SupportedMessages_NONE) {
    return NULL;
  }

  return scheme;
};

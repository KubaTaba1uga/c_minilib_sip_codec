#include "sipmsg/sipmsg.h"

void cmsc_sipmsg_destroy(cmsc_sipmsg_t *sipmsg) {
  if (!sipmsg || !*sipmsg) {
    return;
  }

  free(*sipmsg);
  *sipmsg = NULL;
}

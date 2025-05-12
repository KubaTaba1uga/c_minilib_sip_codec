#include "sipmsg/sipmsg.h"
#include "c_minilib_sip_codec.h"
#include "utils/list.h"

void cmsc_sipmsg_destroy(cmsc_sipmsg_t *sipmsg) {
  if (!sipmsg || !*sipmsg) {
    return;
  }

  struct cmsc_Field_Via *via;
  while (!STAILQ_EMPTY(&(*sipmsg)->via)) {
    via = STAILQ_FIRST(&(*sipmsg)->via);
    STAILQ_REMOVE_HEAD(&(*sipmsg)->via, vias_l);
    free(via);
  }

  free(*sipmsg);
  *sipmsg = NULL;
}

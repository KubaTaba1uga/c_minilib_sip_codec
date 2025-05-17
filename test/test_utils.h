#include <c_minilib_error.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"

inline static void make_msg(const char *raw, struct cmsc_SipMessage **msg) {
  struct cmsc_Buffer buf = {.buf = malloc(strlen(raw) + 1),
                            .len = (uint32_t)strlen(raw),
                            .size = (uint32_t)strlen(raw)};

  TEST_ASSERT_NOT_NULL(buf.buf);
  memcpy((void *)buf.buf, (void *)raw, buf.len);

  cme_error_t err = cmsc_sipmsg_create(buf, msg);
  TEST_ASSERT_NULL(err);
}

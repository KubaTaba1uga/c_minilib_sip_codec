#include <c_minilib_error.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"

inline static void make_msg(const char *raw, struct cmsc_SipMessage **msg) {
  struct cmsc_Buffer buf = {
      .buf = raw, .len = (uint32_t)strlen(raw), .size = (uint32_t)strlen(raw)};

  cme_error_t err = cmsc_sipmsg_create(buf, msg);
  TEST_ASSERT_NULL(err);
}

inline static void create_msg(const char *raw, struct cmsc_SipMessage **msg) {

  const char *raw_buf = malloc(strlen(raw) + 1);
  TEST_ASSERT_NOT_NULL(raw_buf);
  strncpy((void *)raw_buf, raw, strlen(raw));

  struct cmsc_Buffer buf = {.buf = raw_buf,
                            .len = (uint32_t)strlen(raw),
                            .size = (uint32_t)strlen(raw)};

  cme_error_t err = cmsc_sipmsg_create(buf, msg);
  TEST_ASSERT_NULL(err);
}

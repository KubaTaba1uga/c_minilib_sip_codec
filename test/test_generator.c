#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"
#include <string.h>
#include <unity.h>

struct cmsc_SipMessage *msg;
const char *out_buf;

void setUp(void) {
  cme_init();
  msg = NULL;
  out_buf = NULL;
}

void tearDown(void) {
  free((void *)out_buf);
  cmsc_sipmsg_destroy_with_buf(&msg);
  ;
}

void test_generate_invite_exact_match(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  // Set request line: INVITE sip:alice@example.com SIP/2.0
  const char *method = "INVITE";
  const char *uri = "sip:alice@example.com";
  const char *version = "SIP/2.0";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Add one header: X-Test: value
  const char *key = "X-Test";
  const char *val = "value";
  TEST_ASSERT_NULL(
      cmsc_sipmsg_insert_header(strlen(key), key, strlen(val), val, msg));

  // Generate SIP message
  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  // Expected message string (note \r\n line endings)
  const char *expected = "INVITE sip:alice@example.com SIP/2.0\r\n"
                         "X-Test: value\r\n";

  // Match exactly
  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

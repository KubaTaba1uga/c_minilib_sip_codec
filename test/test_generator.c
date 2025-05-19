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
                         "X-Test: value\r\n\r\n";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

void test_generate_full_sip_request(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  const char *method = "INVITE";
  const char *uri = "sip:bob@example.com";
  const char *version = "SIP/2.0";

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("To"), "To",
                                             strlen("<sip:bob@example.com>"),
                                             "<sip:bob@example.com>", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(
      strlen("From"), "From", strlen("<sip:alice@example.com>;tag=123"),
      "<sip:alice@example.com>;tag=123", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("Call-ID"), "Call-ID",
                                             strlen("a84b4c76e66710"),
                                             "a84b4c76e66710", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(
      strlen("CSeq"), "CSeq", strlen("314159 INVITE"), "314159 INVITE", msg));

  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  const char *expected = "INVITE sip:bob@example.com SIP/2.0\r\n"
                         "To: <sip:bob@example.com>\r\n"
                         "From: <sip:alice@example.com>;tag=123\r\n"
                         "Call-ID: a84b4c76e66710\r\n"
                         "CSeq: 314159 INVITE\r\n\r\n";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}
void test_generate_very_long_sip_message(void) {
  // Create SIP message
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  // Insert request line
  const char *method = "OPTIONS";
  const char *uri = "sip:longmsg@example.com";
  const char *version = "SIP/2.0";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Embedded long header value (1023 'X's)
  const char *long_val = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXX";

  // Insert long header
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("X-Long"), "X-Long",
                                             strlen(long_val), long_val, msg));

  // Generate SIP message
  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  // Allocate expected string with snprintf
  char expected_msg[2048];
  snprintf(expected_msg, 2048,
           "OPTIONS sip:longmsg@example.com SIP/2.0\r\n"
           "X-Long: %s\r\n"
           "\r\n",
           long_val);

  // Final assertion
  TEST_ASSERT_EQUAL_STRING(expected_msg, out_buf);
}

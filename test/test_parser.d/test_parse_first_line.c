#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parse_field/parse_status_line.h"
#include "sipmsg/sipmsg.h"

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parse_field/parse_request_line.h"
#include "sipmsg/sipmsg.h"

static cmsc_sipmsg_t msg = NULL;

void setUp(void) {
  cme_init();
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&msg));
}

void tearDown(void) {
  cmsc_sipmsg_destroy(&msg);
  msg = NULL;
  cme_destroy();
}

void test_parse_valid_invite_line(void) {
  const char *line = "INVITE sip:bob@example.com SIP/2.0\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_request_line(len, line, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_REQUEST_LINE));
  TEST_ASSERT_EQUAL_STRING("sip:bob@example.com",
                           msg->request_line.request_uri);
  TEST_ASSERT_EQUAL(2, msg->request_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->request_line.sip_proto_ver.minor);
}

void test_parse_valid_options_line(void) {
  const char *line = "OPTIONS sip:server.domain SIP/2.0\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_request_line(len, line, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_REQUEST_LINE));
  TEST_ASSERT_EQUAL_STRING("sip:server.domain", msg->request_line.request_uri);
  TEST_ASSERT_EQUAL(2, msg->request_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->request_line.sip_proto_ver.minor);
}

void test_parse_line_missing_space_after_method_fails(void) {
  const char *line = "INVITEsip:bob@example.com SIP/2.0\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_request_line(len, line, &msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_line_missing_sip_version_fails(void) {
  const char *line = "INVITE sip:bob@example.com PROTO/2.0\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_request_line(len, line, &msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_valid_status_line_200_ok(void) {
  const char *line = "SIP/2.0 200 OK\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_status_line(len, line, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_STATUS_LINE));
  TEST_ASSERT_EQUAL(200, msg->status_line.status_code);
  TEST_ASSERT_EQUAL_STRING("OK", msg->status_line.reason_phrase);
  TEST_ASSERT_EQUAL(2, msg->status_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->status_line.sip_proto_ver.minor);
}

void test_parse_valid_status_line_with_text_phrase(void) {
  const char *line = "SIP/2.0 486 Busy Here\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_status_line(len, line, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(486, msg->status_line.status_code);
  TEST_ASSERT_EQUAL_STRING("Busy Here", msg->status_line.reason_phrase);
}

void test_parse_status_line_missing_space_after_proto_fails(void) {
  const char *line = "SIP/2.0200 OK\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_status_line(len, line, &msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_status_line_invalid_proto_version_fails(void) {
  const char *line = "INVALID 200 OK\r\n";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parser_parse_status_line(len, line, &msg);
  TEST_ASSERT_NOT_NULL(err);
}

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/supported_messages.h"

static void prepare_line(const char *line, struct cmsc_Line *out_line) {
  struct cmsc_LineIterator iter;
  TEST_ASSERT_NULL(cmsc_line_iterator_init(line, strlen(line), &iter));
  TEST_ASSERT_NOT_NULL(cmsc_line_iterator_next(&iter, out_line));
}

void setUp(void) {
  cme_init();
  cmsc_supmsg_init();
}

void tearDown(void) {
  cmsc_supmsg_destroy();
  cme_destroy();
}

void test_parse_valid_invite_line_first_line(void) {
  struct cmsc_Line line;
  prepare_line("INVITE sip:bob@example.com SIP/2.0\r\n", &line);

  cmsc_sipmsg_t msg = cmsc_parser_parse_first_line(&line);
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_IS_REQUEST));
  TEST_ASSERT_TRUE(msg->is_request);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_REQUEST_LINE));
  TEST_ASSERT_EQUAL(2, msg->request_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->request_line.sip_proto_ver.minor);
  TEST_ASSERT_EQUAL_STRING("sip:bob@example.com",
                           msg->request_line.request_uri);

  cmsc_sipmsg_destroy(&msg);
}

void test_parse_valid_200_ok_line_first_line(void) {
  struct cmsc_Line line;
  prepare_line("SIP/2.0 200 OK\r\n", &line);

  cmsc_sipmsg_t msg = cmsc_parser_parse_first_line(&line);
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_IS_REQUEST));
  TEST_ASSERT_FALSE(msg->is_request);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_200_OK, msg->supmsg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_STATUS_LINE));
  TEST_ASSERT_EQUAL(2, msg->status_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->status_line.sip_proto_ver.minor);
  TEST_ASSERT_EQUAL(200, msg->status_line.status_code);
  TEST_ASSERT_EQUAL_STRING("OK", msg->status_line.reason_phrase);

  cmsc_sipmsg_destroy(&msg);
}

void test_parse_missing_sip_version_first_line(void) {
  struct cmsc_Line line;
  prepare_line("FOO bar@example.com\r\n", &line);

  cmsc_sipmsg_t msg = cmsc_parser_parse_first_line(&line);
  TEST_ASSERT_NULL(msg);
}

void test_parse_unsupported_method_first_line(void) {
  struct cmsc_Line line;
  prepare_line("UNSUPPORTED sip:alice@wonderland.org SIP/2.0\r\n", &line);

  cmsc_sipmsg_t msg = cmsc_parser_parse_first_line(&line);
  TEST_ASSERT_NULL(msg);
}

void test_parse_partial_line_returns_null_first_line(void) {
  struct cmsc_Line line;
  prepare_line("", &line);

  cmsc_sipmsg_t msg = cmsc_parser_parse_first_line(&line);
  TEST_ASSERT_NULL(msg);
}

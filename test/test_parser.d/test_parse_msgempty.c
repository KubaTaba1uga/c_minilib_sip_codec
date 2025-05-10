#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"

static cmsc_parser_t parser = NULL;

void setUp(void) {
  cme_init();
  cmsc_parser_create(&parser);
}

void tearDown(void) {
  cmsc_parser_destroy(&parser);
  parser = NULL;
  cme_destroy();
}

void test_parse_valid_invite_line(void) {
  const char *line = "INVITE sip:bob@example.com SIP/2.0\r\n";
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };

  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_IS_REQUEST));
  TEST_ASSERT_TRUE(msg->is_request);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SIP_PROTO_VER));
  TEST_ASSERT_EQUAL(2, msg->sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->sip_proto_ver.minor);
}

void test_parse_valid_200_ok_line(void) {
  const char *line = "SIP/2.0 200 OK\r\n";
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };

  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_IS_REQUEST));
  TEST_ASSERT_FALSE(msg->is_request);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_200_OK, msg->supmsg);
  TEST_ASSERT_EQUAL(2, msg->sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->sip_proto_ver.minor);
}

void test_parse_missing_sip_version_returns_error(void) {
  const char *line = "FOO bar@example.com\r\n";
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };

  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_unsupported_method_returns_error(void) {
  const char *line = "UNSUPPORTED sip:alice@wonderland.org SIP/2.0\r\n";
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };

  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NOT_NULL(err);
}

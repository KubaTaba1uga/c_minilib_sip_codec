#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/supported_messages.h"

static cmsc_parser_t parser = NULL;

void setUp(void) {
  cme_init();
  cmsc_parser_create(&parser);
  cmsc_supmsg_init();
}

void tearDown(void) {
  cmsc_supmsg_destroy();
  cmsc_parser_destroy(&parser);
  parser = NULL;
  cme_destroy();
}

static void prepare_parser(const char *line, struct cmsc_HeaderIterator *iter) {
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };
  TEST_ASSERT_NULL(cmsc_dynbuf_put(view.buf_len, (char *)view.buf,
                                   (void **)&parser, sizeof(struct cmsc_Parser),
                                   &parser->content));
  TEST_ASSERT_NULL(cmsc_headeriter_init(iter));
}

void test_parse_valid_invite_line_msgempty(void) {
  struct cmsc_HeaderIterator header_iter;
  prepare_parser("INVITE sip:bob@example.com SIP/2.0\r\n", &header_iter);

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_msgempty(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(is_next);

  cmsc_sipmsg_t msg = parser->msg;
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
}

void test_parse_valid_200_ok_line_msgempty(void) {
  struct cmsc_HeaderIterator header_iter;
  prepare_parser("SIP/2.0 200 OK\r\n", &header_iter);

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_msgempty(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(is_next);

  cmsc_sipmsg_t msg = parser->msg;
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
}

void test_parse_missing_sip_version_msgempty(void) {
  struct cmsc_HeaderIterator header_iter;
  prepare_parser("FOO bar@example.com\r\n", &header_iter);

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_msgempty(&header_iter, parser, &is_next);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_FALSE(is_next);
}

void test_parse_unsupported_method_msgempty(void) {
  struct cmsc_HeaderIterator header_iter;
  prepare_parser("UNSUPPORTED sip:alice@wonderland.org SIP/2.0\r\n",
                 &header_iter);

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_msgempty(&header_iter, parser, &is_next);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_FALSE(is_next);
}

void test_parse_partial_line_returns_zero_msgempty(void) {
  struct cmsc_HeaderIterator header_iter;
  prepare_parser("", &header_iter);

  bool is_next = true; // Should be reset
  cme_error_t err = cmsc_parser_parse_msgempty(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err); // not an error, just no progress
  TEST_ASSERT_FALSE(is_next);
}

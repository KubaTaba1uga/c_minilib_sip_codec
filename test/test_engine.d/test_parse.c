// test/test_sip_proto_parse.d/test_sip_proto_parse.c
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "sip_proto/sip_modules/sip_field/parse/parse.h"
#include "sip_proto/sip_proto.h"
struct cmsc_SipMessage *msg = NULL;

#define TEST_BUFFER_SIZE 1024

void setUp(void) {
  cme_init();
  cmsc_sip_proto_init();

  cme_error_t err = cmsc_message_create(&msg, TEST_BUFFER_SIZE);
  TEST_ASSERT_NULL(err);
}

void tearDown(void) {
  free(msg);
  msg = NULL;
  cmsc_sip_proto_destroy();
  cme_destroy();
}

void test_parse_minimal_invite(void) {
  const char *raw_msg = "INVITE sip:bob@example.com SIP/2.0\r\n"
                        "To: alice@example.com\r\n"
                        "\r\n";
  cme_error_t err = cmsc_sip_proto_parse(strlen(raw_msg), raw_msg, msg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(msg->is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, msg->sip_msg_type);
  TEST_ASSERT_EQUAL(2, msg->sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->sip_proto_ver.minor);

  uint32_t expected_mask = cmsc_SipField_IS_REQUEST |
                           cmsc_SipField_SIP_MSG_TYPE |
                           cmsc_SipField_SIP_PROTO_VER;
  TEST_ASSERT_EQUAL(expected_mask, msg->present_mask);
}

void test_parse_to_with_tag(void) {
  const char *line = "<sip:bob@domain.com>;tag=abc123";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->to.tag);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("tag=abc123", msg->to.tag);
  TEST_ASSERT_EQUAL_STRING("<sip:bob@domain.com>;", msg->to.uri);
}

void test_parse_to_without_tag(void) {
  const char *line = "<sip:bob@domain.com>";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(msg->to.tag);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("<sip:bob@domain.com>", msg->to.uri);
}

void test_parse_with_headers(void) {
  const char *raw_msg = "INVITE sip:bob@example.com SIP/2.0\r\n"
                        "To: alice@example.com\r\n"
                        "From: bob@example.com\r\n"
                        "\r\n";
  TEST_ASSERT_NULL(cmsc_sip_proto_parse(strlen(raw_msg), raw_msg, msg));
  TEST_ASSERT_TRUE(msg->is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, msg->sip_msg_type);

  TEST_ASSERT_EQUAL_STRING("alice@example.com", msg->to.uri);
}

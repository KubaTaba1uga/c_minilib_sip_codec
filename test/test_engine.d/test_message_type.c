#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "engine/cmsc_engine.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_valid_invite_request() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, &sipmsg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(sipmsg.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMethod_INVITE, sipmsg.sip_method);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, sipmsg.sip_msg_type);
  TEST_ASSERT_EQUAL(2, sipmsg.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, sipmsg.sip_proto_ver.minor);
}

void test_parse_valid_response() {
  const char *msg = "SIP/2.0 200 OK\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, &sipmsg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_FALSE(sipmsg.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMethod_NONE, sipmsg.sip_method);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_200_OK, sipmsg.sip_msg_type);
  TEST_ASSERT_EQUAL(2, sipmsg.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, sipmsg.sip_proto_ver.minor);
}

void test_parse_unknown_method() {
  const char *msg = "UNKNOWN sip:test@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, &sipmsg);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_TRUE(sipmsg.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_NONE, sipmsg.sip_msg_type);
}

void test_parse_null_buffer() {
  struct cmsc_SipMessage sipmsg = {0};
  cme_error_t err = cmsc_engine_parse_first_line(10, NULL, &sipmsg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_null_msg() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_zero_length() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(0, msg, &sipmsg);
  TEST_ASSERT_NOT_NULL(err);
}

#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "engine/cmsc_engine.h"

void setUp(void) {
  cme_init();
  cmsc_engine_init();
}
void tearDown(void) {
  cmsc_engine_destroy();
  cme_destroy();
}

void test_parse_valid_invite_request(void) {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, &sipmsg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(sipmsg.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, sipmsg.sip_msg_type);
  TEST_ASSERT_EQUAL(2, sipmsg.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, sipmsg.sip_proto_ver.minor);
}

void test_parse_valid_response(void) {
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

void test_parse_unknown_method(void) {
  const char *msg = "UNKNOWN sip:test@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage sipmsg = {0};

  cme_error_t err = cmsc_engine_parse_first_line(strlen(msg), msg, &sipmsg);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_TRUE(sipmsg.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_NONE, sipmsg.sip_msg_type);
}

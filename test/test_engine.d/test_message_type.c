#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

#include "engine/message_type/cmsc_message_type.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_valid_invite_request() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  enum cmsc_SipMsgType type = cmsc_SipMsgType_NONE;
  cme_error_t err = cmsc_message_type_parse(strlen(msg), msg, &type);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, type);
}

void test_parse_valid_response() {
  const char *msg = "SIP/2.0 200 OK\r\n";
  enum cmsc_SipMsgType type = cmsc_SipMsgType_NONE;
  cme_error_t err = cmsc_message_type_parse(strlen(msg), msg, &type);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_200_OK, type);
}

void test_parse_unknown_method() {
  const char *msg = "UNKNOWN sip:test@example.com SIP/2.0\r\n";
  enum cmsc_SipMsgType type = cmsc_SipMsgType_INVITE;
  cme_error_t err = cmsc_message_type_parse(strlen(msg), msg, &type);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_NONE, type);
}

void test_parse_null_buffer() {
  enum cmsc_SipMsgType type;
  cme_error_t err = cmsc_message_type_parse(10, NULL, &type);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_null_type_ptr() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  cme_error_t err = cmsc_message_type_parse(strlen(msg), msg, NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_zero_length() {
  const char *msg = "INVITE sip:alice@example.com SIP/2.0\r\n";
  enum cmsc_SipMsgType type = cmsc_SipMsgType_NONE;
  cme_error_t err = cmsc_message_type_parse(0, msg, &type);
  TEST_ASSERT_NOT_NULL(err);
}

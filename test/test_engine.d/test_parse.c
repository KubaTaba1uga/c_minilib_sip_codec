// test/test_sip_proto_parse.d/test_sip_proto_parse.c
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "sip_proto/sip_proto.h"
#include <c_minilib_error.h>

void setUp(void) {
  cme_init();
  cmsc_sip_proto_init();
}

void tearDown(void) {
  cmsc_sip_proto_destroy();
  cme_destroy();
}

void test_parse_minimal_invite(void) {
  const char *msg = "INVITE sip:bob@example.com SIP/2.0\r\n\r\n";
  struct cmsc_SipMessage *m = NULL;
  cme_error_t err = cmsc_message_create(&m);
  TEST_ASSERT_NULL(err);

  err = cmsc_sip_proto_parse(strlen(msg), msg, m);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(m->is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, m->sip_msg_type);
  TEST_ASSERT_EQUAL(2, m->sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, m->sip_proto_ver.minor);

  uint32_t expected_mask = cmsc_SipField_IS_REQUEST |
                           cmsc_SipField_SIP_MSG_TYPE |
                           cmsc_SipField_SIP_PROTO_VER;
  TEST_ASSERT_EQUAL(expected_mask, m->present_mask);

  cmsc_message_destroy(&m);
}

void test_parse_with_headers(void) {
  const char *msg = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: alice@example.com\r\n"
                    "From: bob@example.com\r\n"
                    "\r\n";
  struct cmsc_SipMessage *m = NULL;
  TEST_ASSERT_NULL(cmsc_message_create(&m));

  TEST_ASSERT_NULL(cmsc_sip_proto_parse(strlen(msg), msg, m));
  TEST_ASSERT_TRUE(m->is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, m->sip_msg_type);
  // headers are currently not extracted, but parse should still succeed

  cmsc_message_destroy(&m);
}

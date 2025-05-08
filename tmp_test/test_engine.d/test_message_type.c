#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "sip_proto/common_sip_proto.h"
#include "sip_proto/sip_modules/invite.h"
#include "sip_proto/sip_modules/ok_200.h"
#include <c_minilib_error.h>

static struct cmsc_Scheme *schemes[2] = {NULL, NULL};
static const struct cmsc_Scheme **schemes_const =
    (const struct cmsc_Scheme **)schemes;
static const uint32_t schemes_len = 2;
cme_error_t err;
void setUp(void) {
  cme_init();

  cme_error_t err = cmsc_sip_proto_invite_init(&schemes[0]);
  TEST_ASSERT_NULL_MESSAGE(err, "Failed to init INVITE scheme");

  err = cmsc_sip_proto_ok_200_init(&schemes[1]);
  TEST_ASSERT_NULL_MESSAGE(err, "Failed to init 200 OK scheme");
}

void tearDown(void) {
  cmsc_sip_proto_invite_destroy(&schemes[0]);
  cmsc_sip_proto_ok_200_destroy(&schemes[1]);
  cme_destroy();
}

void test_first_line_valid_invite(void) {
  const char *msg = "INVITE sip:bob@example.com SIP/2.0\r\n";
  struct cmsc_SipMessage m = {0};

  cme_error_t err = cmsc_sip_proto_parse_first_line(schemes_len, schemes_const,
                                                    strlen(msg), msg, &m);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(m.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, m.sip_msg_type);
  TEST_ASSERT_EQUAL(2, m.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, m.sip_proto_ver.minor);
}

void test_first_line_valid_200_ok(void) {
  const char *msg = "SIP/2.0 200 OK\r\n";
  struct cmsc_SipMessage m = {0};

  cme_error_t err = cmsc_sip_proto_parse_first_line(schemes_len, schemes_const,
                                                    strlen(msg), msg, &m);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_FALSE(m.is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_200_OK, m.sip_msg_type);
  TEST_ASSERT_EQUAL(2, m.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, m.sip_proto_ver.minor);
}

void test_first_line_unknown_type(void) {
  const char *msg = "FOO sip:user@domain SIP/2.0\r\n";
  struct cmsc_SipMessage m = {0};

  cme_error_t err = cmsc_sip_proto_parse_first_line(schemes_len, schemes_const,
                                                    strlen(msg), msg, &m);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_NONE, m.sip_msg_type); // still unset
  TEST_ASSERT_EQUAL(0, m.sip_proto_ver.major);             // still unset
}

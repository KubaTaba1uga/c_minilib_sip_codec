#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/supported_messages.h"

/* You only need the CharBufferView to wrap your input */
static struct cmsc_CharBufferView make_view(const char *s) {
  struct cmsc_CharBufferView v = {.buf = s, .buf_len = strlen(s)};
  return v;
}

static cmsc_parser_t parser = NULL;

void setUp(void) {
  cme_init();
  /* Create parser and register INVITE/200-OK schemes */
  TEST_ASSERT_NULL(cmsc_parser_create(&parser));
  TEST_ASSERT_NULL(cmsc_supmsg_init());
}

void tearDown(void) {
  /* Clean up both parser and schemes */
  cmsc_supmsg_destroy();
  cmsc_parser_destroy(&parser);
  parser = NULL;
  cme_destroy();
}

void test_parse_simple_invite_request(void) {
  /* A minimal SIP INVITE with To and From */
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=123\r\n"
                    "\r\n";

  struct cmsc_CharBufferView view = make_view(raw);
  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;

  /* Pop the completed message */
  /* cmsc_sipmsg_t msg = cmsc_parser_pop_msg(parser); */
  /* TEST_ASSERT_NOT_NULL(msg); */

  /* Verify it's recognized as a request, and as INVITE */
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);

  /* Check the request‐line was parsed */
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_REQUEST_LINE));
  TEST_ASSERT_EQUAL_STRING("sip:bob@example.com",
                           msg->request_line.request_uri);
  TEST_ASSERT_EQUAL(2, msg->request_line.sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->request_line.sip_proto_ver.minor);

  /* Check the To header */
  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->to.uri);
  TEST_ASSERT_NULL(msg->to.tag);

  /* Check the From header */
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:alice@example.com>", msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("123", msg->from.tag);
}

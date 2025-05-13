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
void test_parse_invite_all_mandatory_fields(void) {
  const char *raw =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "To: <sip:bob@example.com>\r\n"
      "From: <sip:alice@example.com>;tag=456\r\n"
      "Call-ID: abc123@host\r\n"
      "CSeq: 1 INVITE\r\n"
      "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
      "Max-Forwards: 70\r\n"
      "\r\n";

  struct cmsc_CharBufferView view = make_view(raw);
  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_CALL_ID));
  TEST_ASSERT_EQUAL_STRING("abc123@host", msg->call_id);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_CSEQ));
  TEST_ASSERT_EQUAL_UINT32(1, msg->cseq.seq_number);
  TEST_ASSERT_EQUAL_STRING("INVITE", msg->cseq.method);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_VIA_L));
  struct cmsc_Field_Via *via = STAILQ_FIRST(&msg->via);
  TEST_ASSERT_NOT_NULL(via);
  TEST_ASSERT_EQUAL_STRING("pc33.example.com", via->sent_by);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_MAX_FORWARDS));
  TEST_ASSERT_EQUAL(70, msg->max_forwards);
}

void test_parse_invite_with_optional_fields(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=tag456\r\n"
                    "Call-ID: xyz456@host\r\n"
                    "CSeq: 2 INVITE\r\n"
                    "Via: SIP/2.0/UDP pc22.example.net;branch=z9hG4bK9sdf8\r\n"
                    "Max-Forwards: 65\r\n"
                    "User-Agent: UnitTestAgent\r\n"
                    "Subject: SIP Invite Test\r\n"
                    "Organization: UnitTest Inc.\r\n"
                    "Allow: INVITE, ACK, CANCEL, OPTIONS\r\n"
                    "Accept: application/sdp\r\n"
                    "Expires: 180\r\n"
                    "\r\n";

  struct cmsc_CharBufferView view = make_view(raw);
  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_USER_AGENT));
  TEST_ASSERT_EQUAL_STRING("UnitTestAgent", msg->user_agent);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUBJECT));
  TEST_ASSERT_EQUAL_STRING("SIP Invite Test", msg->subject);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ORGANIZATION));
  TEST_ASSERT_EQUAL_STRING("UnitTest Inc.", msg->organization);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ALLOW));
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_INVITE);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_ACK);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_CANCEL);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_OPTIONS);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ACCEPT));
  struct cmsc_Field_Accept *accept = STAILQ_FIRST(&msg->accept);
  TEST_ASSERT_NOT_NULL(accept);
  TEST_ASSERT_EQUAL_STRING("application/sdp", accept->mime);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_EXPIRES));
  TEST_ASSERT_EQUAL(180, msg->expires);
}

void test_parse_invite_missing_from_should_fail(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "Call-ID: missingfrom@host\r\n"
                    "CSeq: 1 INVITE\r\n"
                    "Via: SIP/2.0/UDP test.example.com;branch=z9hG4bKxx\r\n"
                    "Max-Forwards: 70\r\n"
                    "\r\n";

  struct cmsc_CharBufferView view = make_view(raw);
  cme_error_t err = cmsc_parser_feed_data(view, &parser);

  TEST_ASSERT_NOT_NULL(err);
}

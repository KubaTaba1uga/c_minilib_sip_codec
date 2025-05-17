#include <stdlib.h>
#include <string.h>

#include "unity_wrapper.h"
#include <c_minilib_sip_codec.h>

#include "utils.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) { cme_init(); }
void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

static void parse_msg(const char *raw) {
  cme_error_t err = cmsc_parse_sip((uint32_t)strlen(raw), raw, &msg);
  if (err) {
    puts(err->msg);
  }
  TEST_ASSERT_NULL(err);
}

void test_parse_basic_invite_request(void) {
  const char *raw =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
      "To: <sip:bob@example.com>\r\n"
      "From: <sip:alice@example.com>;tag=1928301774\r\n"
      "Call-ID: a84b4c76e66710\r\n"
      "CSeq: 314159 INVITE\r\n"
      "Max-Forwards: 13\r\n"
      "X-Custom:         Some custom value       \r\n"
      "\r\n";

  parse_msg(raw);

  // Request line checks
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                                 msg->request_line.sip_method.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com",
                                 msg->request_line.request_uri.buf,
                                 msg->request_line.request_uri.len);

  // Decoded header fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", msg->to.uri.buf,
                                 msg->to.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", msg->from.uri.buf,
                                 msg->from.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("1928301774", msg->from.tag.buf,
                                 msg->from.tag.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->cseq.method.buf,
                                 msg->cseq.method.len);
  TEST_ASSERT_EQUAL(314159, msg->cseq.seq_number);
  MYTEST_ASSERT_EQUAL_STRING_LEN("a84b4c76e66710", msg->call_id.buf,
                                 msg->call_id.len);
  TEST_ASSERT_EQUAL(13, msg->max_forwards);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z9hG4bK776asdhds",
                                 msg->vias.stqh_first->branch.buf,
                                 msg->vias.stqh_first->branch.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("pc33.example.com",
                                 msg->vias.stqh_first->sent_by.buf,
                                 msg->vias.stqh_first->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("UDP", msg->vias.stqh_first->proto.buf,
                                 msg->vias.stqh_first->proto.len);

  // Remaining headers (only non-decoded should remain)
  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN("X-Custom", h->key.buf, h->key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("Some custom value", h->value.buf,
                                 h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NULL(h); // Ensure only 1 header remained after decoding
}

void test_parse_multiple_via_headers(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "Via: SIP/2.0/UDP first.example.com;branch=z1\r\n"
                    "Via: SIP/2.0/TCP second.example.com;branch=z2,SIP/2.0/WS "
                    "third.example.com;branch=z3\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=taggy\r\n"
                    "Call-ID: abc123\r\n"
                    "CSeq: 1 INVITE\r\n"
                    "Max-Forwards: 70\r\n"
                    "\r\n";

  parse_msg(raw);

  // Check all decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", msg->to.uri.buf,
                                 msg->to.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", msg->from.uri.buf,
                                 msg->from.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("taggy", msg->from.tag.buf, msg->from.tag.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->cseq.method.buf,
                                 msg->cseq.method.len);
  TEST_ASSERT_EQUAL(1, msg->cseq.seq_number);
  MYTEST_ASSERT_EQUAL_STRING_LEN("abc123", msg->call_id.buf, msg->call_id.len);
  TEST_ASSERT_EQUAL(70, msg->max_forwards);

  // Check Via list (should contain 3 entries)
  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&msg->vias);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("UDP", via->proto.buf, via->proto.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("first.example.com", via->sent_by.buf,
                                 via->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z1", via->branch.buf, via->branch.len);

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("TCP", via->proto.buf, via->proto.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("second.example.com", via->sent_by.buf,
                                 via->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z2", via->branch.buf, via->branch.len);

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("WS", via->proto.buf, via->proto.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("third.example.com", via->sent_by.buf,
                                 via->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z3", via->branch.buf, strlen("z3"));

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NULL(via); // Only 3 vias expected
}

void test_parse_message_with_body(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=tag123\r\n"
                    "Call-ID: call123\r\n"
                    "CSeq: 1 INVITE\r\n"
                    "Content-Length: 16\r\n"
                    "\r\n"
                    "Hello from body!";

  parse_msg(raw);

  // Basic field checks
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", msg->to.uri.buf,
                                 msg->to.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", msg->from.uri.buf,
                                 msg->from.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("tag123", msg->from.tag.buf,
                                 msg->from.tag.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->cseq.method.buf,
                                 msg->cseq.method.len);
  TEST_ASSERT_EQUAL(1, msg->cseq.seq_number);
  MYTEST_ASSERT_EQUAL_STRING_LEN("call123", msg->call_id.buf, msg->call_id.len);

  // Check body
  printf("body: %.*s\n", msg->body.len, msg->body.buf);
  TEST_ASSERT_EQUAL(16, msg->body.len);
  TEST_ASSERT_EQUAL(16, msg->content_length);
  MYTEST_ASSERT_EQUAL_STRING_LEN("Hello from body!", msg->body.buf,
                                 msg->body.len);
}

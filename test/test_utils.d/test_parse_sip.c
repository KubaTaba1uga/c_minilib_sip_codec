#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <c_minilib_sip_codec.h>

#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {}
void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

static void parse_msg(const char *raw) {
  cme_error_t err = cmsc_parse_sip((uint32_t)strlen(raw), raw, &msg);
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
      "\r\n";

  parse_msg(raw);

  // Request line checks
  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                               msg->request_line.sip_method.len);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com",
                               msg->request_line.request_uri.buf,
                               msg->request_line.request_uri.len);

  // Decoded header fields
  TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", msg->to.uri.buf, msg->to.uri.len);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", msg->from.uri.buf, msg->from.uri.len);
  TEST_ASSERT_EQUAL_STRING_LEN("1928301774", msg->from.tag.buf, msg->from.tag.len);
  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->cseq.method.buf, msg->cseq.method.len);
  TEST_ASSERT_EQUAL(314159, msg->cseq.seq_number);
  TEST_ASSERT_EQUAL_STRING_LEN("a84b4c76e66710", msg->call_id.buf, msg->call_id.len);
  
  // Remaining headers (only non-decoded should remain)
  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("Via", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds",
                               h->value.buf, h->value.len);


  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NULL(h); // Ensure only 3 headers remain after decoding
}


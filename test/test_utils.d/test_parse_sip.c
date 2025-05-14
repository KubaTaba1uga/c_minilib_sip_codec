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
      "To: Bob <sip:bob@example.com>\r\n"
      "From: Alice <sip:alice@example.com>;tag=1928301774\r\n"
      "Call-ID: a84b4c76e66710\r\n"
      "CSeq: 314159 INVITE\r\n"
      "\r\n";

  parse_msg(raw);

  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                               msg->request_line.sip_method.len);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com",
                               msg->request_line.request_uri.buf,
                               msg->request_line.request_uri.len);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);

  TEST_ASSERT_EQUAL_STRING_LEN("Via", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds", h->value.buf, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("To", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" Bob <sip:bob@example.com>", h->value.buf, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("From", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" Alice <sip:alice@example.com>;tag=1928301774", h->value.buf, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("Call-ID", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" a84b4c76e66710", h->value.buf, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("CSeq", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" 314159 INVITE", h->value.buf, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NULL(h); // Ensure only 5 headers were parsed
}


/* void test_parse_basic_invite_request(void) { */
/*   const char *raw = */
/*       "INVITE sip:bob@example.com SIP/2.0\r\n" */
/*       "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n" */
/*       "To: Bob <sip:bob@example.com>\r\n" */
/*       "From: Alice <sip:alice@example.com>;tag=1928301774\r\n" */
/*       "Call-ID: a84b4c76e66710\r\n" */
/*       "CSeq: 314159 INVITE\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf, */
/*                                msg->request_line.sip_method.len); */
/*   TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", */
/*                                msg->request_line.request_uri.buf, */
/*                                msg->request_line.request_uri.len); */
/* } */

/* void test_parse_register_request(void) { */
/*   const char *raw = */
/*       "REGISTER sip:example.com SIP/2.0\r\n" */
/*       "Via: SIP/2.0/UDP host.example.com;branch=z9hG4bK776asdhds\r\n" */
/*       "To: <sip:alice@example.com>\r\n" */
/*       "From: <sip:alice@example.com>;tag=12345\r\n" */
/*       "Call-ID: reg1234\r\n" */
/*       "CSeq: 1 REGISTER\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL_STRING_LEN("REGISTER", msg->request_line.sip_method.buf,
 */
/*                                msg->request_line.sip_method.len); */
/*   TEST_ASSERT_EQUAL_STRING_LEN("sip:example.com", */
/*                                msg->request_line.request_uri.buf, */
/*                                msg->request_line.request_uri.len); */
/* } */

/* void test_parse_200_ok_response(void) { */
/*   const char *raw = */
/*       "SIP/2.0 200 OK\r\n" */
/*       "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n" */
/*       "To: Bob <sip:bob@example.com>;tag=2493k59kd\r\n" */
/*       "From: Alice <sip:alice@example.com>;tag=1928301774\r\n" */
/*       "Call-ID: a84b4c76e66710\r\n" */
/*       "CSeq: 314159 INVITE\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL(200, msg->status_line.status_code); */
/*   TEST_ASSERT_EQUAL_STRING_LEN("OK", msg->status_line.reason_phrase.buf, */
/*                                msg->status_line.reason_phrase.len); */
/* } */

/* void test_parse_486_busy_here(void) { */
/*   const char *raw = */
/*       "SIP/2.0 486 Busy Here\r\n" */
/*       "Via: SIP/2.0/UDP server10.example.com;branch=z9hG4bKnashds8\r\n" */
/*       "To: Bob <sip:bob@example.com>;tag=a6c85cf\r\n" */
/*       "From: Alice <sip:alice@example.com>;tag=1928301774\r\n" */
/*       "Call-ID: a84b4c76e66710\r\n" */
/*       "CSeq: 314159 INVITE\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL(486, msg->status_line.status_code); */
/*   TEST_ASSERT_EQUAL_STRING_LEN("Busy Here",
 * msg->status_line.reason_phrase.buf, */
/*                                msg->status_line.reason_phrase.len); */
/* } */

/* void test_parse_100_no_reason_phrase(void) { */
/*   const char *raw = */
/*       "SIP/2.0 100 \r\n" */
/*       "Via: SIP/2.0/UDP test.com\r\n" */
/*       "To: <sip:foo@test.com>\r\n" */
/*       "From: <sip:bar@test.com>;tag=aaa\r\n" */
/*       "Call-ID: testid\r\n" */
/*       "CSeq: 1 TRYING\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL(100, msg->status_line.status_code); */
/*   TEST_ASSERT_EQUAL(0, msg->status_line.reason_phrase.len); */
/* } */

/* void test_parse_request_with_extra_spaces(void) { */
/*   const char *raw = */
/*       "INVITE   sip:bob@example.com   SIP/2.0\r\n" */
/*       "Via: SIP/2.0/UDP host\r\n" */
/*       "To: <sip:bob@example.com>\r\n" */
/*       "From: <sip:alice@example.com>;tag=xyz\r\n" */
/*       "Call-ID: call123\r\n" */
/*       "CSeq: 10 INVITE\r\n" */
/*       "\r\n"; */

/*   parse_msg(raw); */

/*   TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf, */
/*                                msg->request_line.sip_method.len); */
/*   TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", */
/*                                msg->request_line.request_uri.buf, */
/*                                msg->request_line.request_uri.len); */
/* } */

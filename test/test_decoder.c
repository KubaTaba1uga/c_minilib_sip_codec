#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "unity_wrapper.h"
#include "utils.h"
#include "utils/decoder.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {}
void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

void test_decode_to_header(void) {
  const char *raw_to_value = "<sip:bob@example.com>;tag=123abc";
  cme_error_t err;

  make_msg(raw_to_value, &msg);

  // Allocate and insert To header
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "To";
  hdr->key.len = 2;
  hdr->value.buf = raw_to_value;
  hdr->value.len = (uint32_t)strlen(raw_to_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Ensure header was consumed
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Validate decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com", msg->to.uri.buf,
                                 msg->to.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("123abc", msg->to.tag.buf, msg->to.tag.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_TO));
}

void test_decode_from_header(void) {
  const char *raw_to_value = "sip:alice@example.com";
  cme_error_t err;

  make_msg(raw_to_value, &msg);
  // Allocate and insert To header
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "From";
  hdr->key.len = 4;
  hdr->value.buf = raw_to_value;
  hdr->value.len = (uint32_t)strlen(raw_to_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Ensure header was consumed
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Validate decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", msg->from.uri.buf,
                                 msg->from.uri.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_FROM));
}

void test_decode_cseq_header(void) {
  const char *raw_value = "42 INVITE";
  cme_error_t err;

  make_msg(raw_value, &msg);
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "CSeq";
  hdr->key.len = 4;
  hdr->value.buf = raw_value;
  hdr->value.len = (uint32_t)strlen(raw_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(42, msg->cseq.seq_number);
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->cseq.method.buf,
                                 msg->cseq.method.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_CSEQ));
}

void test_decode_call_id_header(void) {
  const char *raw_value = "abcd-1234";
  cme_error_t err;

  make_msg(raw_value, &msg);

  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "Call-ID";
  hdr->key.len = 7;
  hdr->value.buf = raw_value;
  hdr->value.len = (uint32_t)strlen(raw_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN("abcd-1234", msg->call_id.buf,
                                 msg->call_id.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_CALL_ID));
}

void test_decode_max_forwards_header(void) {
  const char *raw_value = "70";
  cme_error_t err;

  make_msg(raw_value, &msg);
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "Max-Forwards";
  hdr->key.len = 12;
  hdr->value.buf = raw_value;
  hdr->value.len = (uint32_t)strlen(raw_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(70, msg->max_forwards);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_MAX_FORWARDS));
}

void test_decode_via_header_single(void) {
  const char *raw_value = "SIP/2.0/UDP host.example.com;branch=z9hG4bK";
  cme_error_t err;

  make_msg(raw_value, &msg);
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "Via";
  hdr->key.len = 3;
  hdr->value.buf = raw_value;
  hdr->value.len = (uint32_t)strlen(raw_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&msg->vias);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("UDP", via->proto.buf, via->proto.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("host.example.com", via->sent_by.buf,
                                 via->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z9hG4bK", via->branch.buf, via->branch.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_VIAS));
}

void test_decode_content_length_header(void) {
  const char *raw_value = "123";
  cme_error_t err;

  make_msg(raw_value, &msg);

  // Allocate and insert Content-Length header
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  hdr->key.buf = "Content-Length";
  hdr->key.len = (uint32_t)strlen("Content-Length");
  hdr->value.buf = raw_value;
  hdr->value.len = (uint32_t)strlen(raw_value);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Custom check: if implemented in future decoder logic
  // For now just confirm that header was not erroneously left
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Optional: verify value if `content_length` field is added to
  // `cmsc_SipMessage`
  TEST_ASSERT_EQUAL(123, msg->content_length);
}

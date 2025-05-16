#include <stdlib.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "unity_wrapper.h"
#include "utils/decoder.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {}
void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

void test_decode_to_header(void) {
  const char *raw_to_value = "<sip:bob@example.com>;tag=123abc";

  // Allocate and initialize the SIP message
  struct cmsc_Buffer buf = {.buf = raw_to_value,
                            .len = (uint32_t)strlen(raw_to_value),
                            .size = (uint32_t)strlen(raw_to_value)};

  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

void test_decode_from_header(void) {
  const char *raw_to_value = "sip:alice@example.com";

  // Allocate and initialize the SIP message
  struct cmsc_Buffer buf = {.buf = raw_to_value,
                            .len = (uint32_t)strlen(raw_to_value),
                            .size = (uint32_t)strlen(raw_to_value)};

  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

void test_decode_cseq_header(void) {
  const char *raw_value = "42 INVITE";

  struct cmsc_Buffer buf = {.buf = raw_value,
                            .len = (uint32_t)strlen(raw_value),
                            .size = (uint32_t)strlen(raw_value)};
  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

void test_decode_call_id_header(void) {
  const char *raw_value = "abcd-1234";

  struct cmsc_Buffer buf = {.buf = raw_value,
                            .len = (uint32_t)strlen(raw_value),
                            .size = (uint32_t)strlen(raw_value)};
  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

void test_decode_max_forwards_header(void) {
  const char *raw_value = "70";

  struct cmsc_Buffer buf = {.buf = raw_value,
                            .len = (uint32_t)strlen(raw_value),
                            .size = (uint32_t)strlen(raw_value)};
  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

void test_decode_via_header_single(void) {
  const char *raw_value = "SIP/2.0/UDP host.example.com;branch=z9hG4bK";

  struct cmsc_Buffer buf = {.buf = raw_value,
                            .len = (uint32_t)strlen(raw_value),
                            .size = (uint32_t)strlen(raw_value)};
  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_NULL(err);

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
}

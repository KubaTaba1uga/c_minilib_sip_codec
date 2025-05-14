#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <c_minilib_sip_codec.h>

#include "utils/parser.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {}

void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

static void make_msg(const char *raw) {
  struct cmsc_Buffer buf = {
      .buf = raw, .len = (uint32_t)strlen(raw), .size = (uint32_t)strlen(raw)};

  cme_error_t err = cmsc_sipmsg_create(buf, &msg);
  TEST_ASSERT_EQUAL(0, err);
}

void test_parse_null_msg(void) {
  cme_error_t err = cmsc_parse_sip_headers(NULL);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_parse_single_header(void) {
  make_msg("Content-Type: application/sdp\r\n");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);

  TEST_ASSERT_EQUAL_STRING_LEN("Content-Type", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL_STRING_LEN(" application/sdp", h->value.buf, h->value.len);
}

void test_parse_multiple_headers(void) {
  make_msg("Content-Type: application/sdp\r\n"
           "Content-Length: 123\r\n");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("Content-Type", h->key.buf, h->key.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("Content-Length", h->key.buf, h->key.len);
}

void test_parse_empty_buffer(void) {
  make_msg("");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_header_without_value(void) {
  make_msg("X-Empty:\r\n");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("X-Empty", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL(0, h->value.len); // No value after ':'
}

void test_parse_header_with_lf_only(void) {
  make_msg("X-Bad: value\n"); // Missing \r

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  // Should skip the header — no \r\n means not valid SIP header end
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_header_without_colon(void) {
  make_msg("BadHeaderNoColon\r\n");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  // No colon means header was never created — should be ignored
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_multiple_mixed_headers(void) {
  make_msg("X-Good: yes\r\n"
           "NoColon\r\n"
           "X-Empty:\r\n"
           "Y-Good: again\r\n"
           "Bad\n");

  cme_error_t err = cmsc_parse_sip_headers(msg);
  TEST_ASSERT_EQUAL(0, err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("X-Good", h->key.buf, h->key.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("X-Empty", h->key.buf, h->key.len);
  TEST_ASSERT_EQUAL(0, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  TEST_ASSERT_EQUAL_STRING_LEN("Y-Good", h->key.buf, h->key.len);

  TEST_ASSERT_NULL(STAILQ_NEXT(h, _next)); // Only 3 valid headers
}

void test_valid_request_line(void) {
  make_msg("INVITE sip:bob@biloxi.com SIP/2.0\r\n");

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                               msg->request_line.sip_method.len);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@biloxi.com",
                               msg->request_line.request_uri.buf,
                               msg->request_line.request_uri.len);
  TEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0", msg->request_line.sip_proto_ver.buf,
                               msg->request_line.sip_proto_ver.len);
}

void test_missing_crlf(void) {
  make_msg("INVITE sip:bob@biloxi.com SIP/2.0"); // No \r\n

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_missing_sip_version(void) {
  make_msg("INVITE sip:bob@biloxi.com\r\n");

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_malformed_method_line(void) {
  make_msg("INVITE123sip:bob@biloxi.com SIP/2.0\r\n");

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_invalid_request_line_extra_space(void) {
  make_msg("INVITE  sip:bob@biloxi.com  SIP/2.0\r\n");

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_EQUAL(0, err); // still valid
  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                               msg->request_line.sip_method.len);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:bob@biloxi.com",
                               msg->request_line.request_uri.buf,
                               msg->request_line.request_uri.len);
  TEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0", msg->request_line.sip_proto_ver.buf,
                               msg->request_line.sip_proto_ver.len);
}

void test_sip_version_not_in_first_line(void) {
  make_msg("Hello world\r\nFoo: bar\r\nSIP/2.0\r\n");

  cme_error_t err = cmsc_parse_sip_first_line(msg);
  TEST_ASSERT_NOT_EQUAL(0, err); // SIP version not in first line
}

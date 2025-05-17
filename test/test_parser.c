#include <stdlib.h>
#include <string.h>

#include <c_minilib_sip_codec.h>
#include <unity_wrapper.h>

#include "c_minilib_error.h"
#include "utils.h"
#include "utils/bstring.h"
#include "utils/parser.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) { cme_init(); }

void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

void test_parse_null_msg(void) {
  cme_error_t err = cmsc_parse_sip_headers(NULL, NULL);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_parse_single_header(void) {
  make_msg("Content-Type: application/sdp\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Content-Type", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN(" application/sdp",
                                 cmsc_bs_msg_to_string(&h->value, msg).buf,
                                 h->value.len);
}

void test_parse_multiple_headers(void) {
  make_msg("Content-Type: application/sdp\r\n"
           "Content-Length: 123\r\n",
           &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Content-Type", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Content-Length", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
}

void test_parse_empty_buffer(void) {
  make_msg("", &msg);

  cme_error_t err = cmsc_parse_sip_headers(&msg->_buf, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_header_without_value(void) {
  make_msg("X-Empty:\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Empty", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
  TEST_ASSERT_EQUAL(0, h->value.len); // No value after ':'
}

void test_parse_header_with_lf_only(void) {
  make_msg("X-Bad: value\n", &msg); // Missing \r

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  // Should skip the header — no \r\n means not valid SIP header end
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_header_without_colon(void) {
  make_msg("BadHeaderNoColon\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  // No colon means header was never created — should be ignored
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));
}

void test_parse_multiple_mixed_headers(void) {
  make_msg("X-Good: yes\r\n"
           "NoColon\r\n"
           "X-Empty:\r\n"
           "Y-Good: again\r\n"
           "Bad\n",
           &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_headers(&parse_buf, msg);

  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Good", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Empty", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
  TEST_ASSERT_EQUAL(0, h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Y-Good", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);

  TEST_ASSERT_NULL(STAILQ_NEXT(h, _next)); // Only 3 valid headers
}

void test_valid_request_line(void) {
  make_msg("INVITE sip:bob@biloxi.com SIP/2.0\r\n", &msg);
  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                                 msg->request_line.sip_method.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@biloxi.com",
                                 msg->request_line.request_uri.buf,
                                 msg->request_line.request_uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0", msg->request_line.sip_proto_ver.buf,
                                 msg->request_line.sip_proto_ver.len);
}

void test_missing_crlf(void) {
  make_msg("INVITE sip:bob@biloxi.com SIP/2.0", &msg); // No \r\n

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_missing_sip_version(void) {
  make_msg("INVITE sip:bob@biloxi.com\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_malformed_method_line(void) {
  make_msg("INVITE123sip:bob@biloxi.com SIP/2.0\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_invalid_request_line_extra_space(void) {
  make_msg("INVITE  sip:bob@biloxi.com  SIP/2.0\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NULL(err); // still valid
  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE", msg->request_line.sip_method.buf,
                                 msg->request_line.sip_method.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@biloxi.com",
                                 msg->request_line.request_uri.buf,
                                 msg->request_line.request_uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0", msg->request_line.sip_proto_ver.buf,
                                 msg->request_line.sip_proto_ver.len);
}

void test_sip_version_not_in_first_line(void) {
  make_msg("Hello world\r\nFoo: bar\r\nSIP/2.0\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NOT_NULL(err); //
}

void test_valid_status_line_parsing(void) {
  make_msg("SIP/2.0 200 OK\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0", msg->status_line.sip_proto_ver.buf,
                                 msg->status_line.sip_proto_ver.len);

  TEST_ASSERT_EQUAL(200, msg->status_line.status_code);

  MYTEST_ASSERT_EQUAL_STRING_LEN("OK", msg->status_line.reason_phrase.buf,
                                 msg->status_line.reason_phrase.len);
}

void test_status_line_with_reason_phrase_spaces(void) {
  make_msg("SIP/2.0 486 Busy Here\r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(486, msg->status_line.status_code);

  MYTEST_ASSERT_EQUAL_STRING_LEN("Busy Here",
                                 msg->status_line.reason_phrase.buf,
                                 msg->status_line.reason_phrase.len);
}

void test_status_line_missing_reason_phrase(void) {
  make_msg("SIP/2.0 100 \r\n", &msg);

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(100, msg->status_line.status_code);
  TEST_ASSERT_EQUAL(0, msg->status_line.reason_phrase.len);
}

void test_status_line_not_numeric_code(void) {
  make_msg("SIP/2.0 twohundred OK\r\n", &msg); // non-numeric

  struct cmsc_Buffer parse_buf = msg->_buf;
  cme_error_t err = cmsc_parse_sip_first_line(&parse_buf, msg);
  TEST_ASSERT_NOT_NULL(err);
}

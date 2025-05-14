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

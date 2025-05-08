#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/line_iterator.h"
#include "utils/dynamic_buffer.h"
#include <c_minilib_error.h>

static struct cmsc_DynamicBuffer *buf = NULL;

static const char *sample_headers =
    "Via: SIP/2.0/UDP server1.example.com\r\n"
    "To: <sip:user@example.com>\r\n"
    "From: <sip:caller@example.net>\r\n"
    "\r\n"
    "This is body data and should not be parsed.";

void setUp(void) {
  cme_init();
  cme_error_t err = cmsc_dynbuf_create(strlen(sample_headers) + 1, &buf);
  TEST_ASSERT_NULL(err);
  err = cmsc_dynbuf_put(strlen(sample_headers), (char *)sample_headers, &buf);
  TEST_ASSERT_NULL(err);
}

void tearDown(void) {
  cmsc_dynbuf_destroy(&buf);
  cme_destroy();
}

void test_headeriter_init_null(void) {
  cme_error_t err = cmsc_headeriter_init(NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_headeriter_init_valid(void) {
  struct cmsc_HeaderIterator iter;
  cme_error_t err = cmsc_headeriter_init(&iter);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(iter.line_start);
  TEST_ASSERT_NULL(iter.line_end);
  TEST_ASSERT_NULL(iter.colon);
}
void test_headeriter_next_walks_headers(void) {
  struct cmsc_HeaderIterator iter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&iter));

  // 1. VIA
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &iter));
  TEST_ASSERT_NOT_NULL(iter.line_start);
  TEST_ASSERT_NOT_NULL(iter.line_end);
  TEST_ASSERT_NOT_NULL(iter.colon);
  TEST_ASSERT_EQUAL_STRING_LEN("Via", iter.line_start, 3);

  // Calculate expected end pointer (start + strlen - 2 for \r\n)
  const char *via_expected_end =
      iter.line_start + strlen("Via: SIP/2.0/UDP server1.example.com");
  TEST_ASSERT_EQUAL_PTR(via_expected_end, iter.line_end);

  // 2. TO
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &iter));
  TEST_ASSERT_EQUAL_STRING_LEN("To", iter.line_start, 2);
  const char *to_expected_end =
      iter.line_start + strlen("To: <sip:user@example.com>");
  TEST_ASSERT_EQUAL_PTR(to_expected_end, iter.line_end);

  // 3. FROM
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &iter));
  TEST_ASSERT_EQUAL_STRING_LEN("From", iter.line_start, 4);
  const char *from_expected_end =
      iter.line_start + strlen("From: <sip:caller@example.net>");
  TEST_ASSERT_EQUAL_PTR(from_expected_end, iter.line_end);

  // 4. End (empty line = body start)
  TEST_ASSERT_FALSE(cmsc_headeriter_next(buf, &iter));
}

void test_headeriter_next_handles_no_colon(void) {
  const char *bad_headers = "MalformedHeader\r\n"
                            "\r\n";

  cmsc_dynbuf_destroy(&buf);
  cme_error_t err = cmsc_dynbuf_create(strlen(bad_headers) + 1, &buf);
  TEST_ASSERT_NULL(err);
  err = cmsc_dynbuf_put(strlen(bad_headers), (char *)bad_headers, &buf);
  TEST_ASSERT_NULL(err);

  struct cmsc_HeaderIterator iter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&iter));
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &iter));
  TEST_ASSERT_NULL(iter.colon);
  TEST_ASSERT_FALSE(cmsc_headeriter_next(buf, &iter));
}

void test_headeriter_next_null_inputs(void) {
  struct cmsc_HeaderIterator iter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&iter));
  TEST_ASSERT_FALSE(cmsc_headeriter_next(NULL, &iter));
  TEST_ASSERT_FALSE(cmsc_headeriter_next(buf, NULL));
  TEST_ASSERT_FALSE(cmsc_headeriter_next(NULL, NULL));
}

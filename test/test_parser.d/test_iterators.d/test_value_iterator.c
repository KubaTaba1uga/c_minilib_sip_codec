#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/dynamic_buffer.h"
#include <c_minilib_error.h>

static struct cmsc_DynamicBuffer *buf = NULL;

void setUp(void) { cme_init(); }

void tearDown(void) {
  cmsc_dynbuf_destroy(&buf);
  cme_destroy();
}

void test_valueiter_init_null(void) {
  cme_error_t err = cmsc_valueiter_init(NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_valueiter_init_valid(void) {
  struct cmsc_ValueIterator viter;
  cme_error_t err = cmsc_valueiter_init(&viter);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(viter.value_start);
  TEST_ASSERT_NULL(viter.value_end);
  TEST_ASSERT_NULL(viter.header_start);
  TEST_ASSERT_EQUAL_UINT32(0, viter.header_len);
}


void test_valueiter_single_value(void) {
  const char *single = "Contact: sip:user@example.com\r\n";
  cmsc_dynbuf_create(strlen(single) + 1, &buf);
  cme_error_t err = cmsc_dynbuf_put(strlen(single), (char *)single, &buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(buf);

  struct cmsc_HeaderIterator hiter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&hiter));
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));

  struct cmsc_ValueIterator viter;
  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));

  TEST_ASSERT_TRUE(cmsc_valueiter_next(&hiter, &viter));
  size_t len = viter.value_end - viter.value_start;
  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", viter.value_start, len);

  TEST_ASSERT_FALSE(cmsc_valueiter_next(&hiter, &viter));
}

void test_valueiter_next_multiple_values(void) {
  const char *header_line =
      "Via: a.example.com, b.example.com, c.example.com\r\n";
  cmsc_dynbuf_create(strlen(header_line) + 1, &buf);
  cme_error_t err =
      cmsc_dynbuf_put(strlen(header_line), (char *)header_line, &buf);
  TEST_ASSERT_NULL(err);

  struct cmsc_HeaderIterator hiter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&hiter));
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));

  struct cmsc_ValueIterator viter;
  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));

  const char *expected_values[] = {"a.example.com", "b.example.com",
                                   "c.example.com"};

  size_t count = 0;
  while (cmsc_valueiter_next(&hiter, &viter)) {
    size_t len = viter.value_end - viter.value_start;
    TEST_ASSERT_EQUAL_STRING_LEN(expected_values[count], viter.value_start,
                                 len);
    count++;
  }

  TEST_ASSERT_EQUAL_UINT32(3, count);
  TEST_ASSERT_EQUAL_STRING_LEN("Via", viter.header_start, viter.header_len);
}


void test_valueiter_mixed_headers(void) {
  const char *headers = "X-Empty:\r\n"
                        "Contact: sip:alice@example.com\r\n"
                        "Via: sip:1@a.example.com, sip:2@b.example.org\r\n"
                        "\r\n";

  cmsc_dynbuf_create(strlen(headers) + 1, &buf);
  cme_error_t err = cmsc_dynbuf_put(strlen(headers), (char *)headers, &buf);
  TEST_ASSERT_NULL(err);

  struct cmsc_HeaderIterator hiter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&hiter));

  struct cmsc_ValueIterator viter;
  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));

  // Header 1: X-Empty (no value)
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));
  TEST_ASSERT_EQUAL_STRING_LEN("X-Empty", hiter.line_start, strlen("X-Empty"));
  TEST_ASSERT_TRUE(cmsc_valueiter_next(&hiter, &viter));

  // Header 2: Contact (one value)
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));
  TEST_ASSERT_EQUAL_STRING_LEN("Contact", hiter.line_start, strlen("Contact"));

  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));  
  TEST_ASSERT_TRUE(cmsc_valueiter_next(&hiter, &viter));
  size_t len = viter.value_end - viter.value_start;
  TEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com", viter.value_start,
                               len);
  TEST_ASSERT_FALSE(cmsc_valueiter_next(&hiter, &viter));

  // Header 3: Via (multiple values)
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));
  TEST_ASSERT_EQUAL_STRING_LEN("Via", hiter.line_start, strlen("Via"));

  const char *expected_via[] = {"sip:1@a.example.com", "sip:2@b.example.org"};
  size_t count = 0;
  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));  
  while (cmsc_valueiter_next(&hiter, &viter)) {
    size_t len = viter.value_end - viter.value_start;
    TEST_ASSERT_EQUAL_STRING_LEN(expected_via[count], viter.value_start, len);
    count++;
  }
  TEST_ASSERT_EQUAL_UINT32(2, count);
  TEST_ASSERT_FALSE(cmsc_headeriter_next(buf, &hiter));
}

void test_valueiter_next_invalid_inputs(void) {
  struct cmsc_ValueIterator viter;
  TEST_ASSERT_NULL(cmsc_valueiter_init(&viter));

  // all null
  TEST_ASSERT_FALSE(cmsc_valueiter_next(NULL, NULL));

  // null headeriter
  TEST_ASSERT_FALSE(cmsc_valueiter_next(NULL, &viter));

  // null valueiter
  const char *line = "X-Test: foo\r\n";
  cmsc_dynbuf_create(strlen(line) + 1, &buf);
  cme_error_t err = cmsc_dynbuf_put(strlen(line), (char *)line, &buf);
  TEST_ASSERT_NULL(err);

  struct cmsc_HeaderIterator hiter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&hiter));
  TEST_ASSERT_TRUE(cmsc_headeriter_next(buf, &hiter));
  TEST_ASSERT_FALSE(cmsc_valueiter_next(&hiter, NULL));
}

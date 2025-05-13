#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/value_iterator.h"
#include "utils/string.h"

static const char *test_buf = "Via: host1;branch=xyz, host2;branch=abc\r\n"
                              "Allow: INVITE, ACK, BYE\r\n"
                              "InvalidHeader\r\n";

void test_value_iterator_single_header_with_values(void) {
  struct cmsc_ValueIterator iter;
  struct cmsc_ValueLine value = {0};

  const char *buf = "Allow: INVITE, ACK\r\n";
  cme_error_t err = cmsc_value_iterator_init(buf, strlen(buf), &iter);
  TEST_ASSERT_NULL(err);

  // First value (INVITE)
  struct cmsc_ValueLine *v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("Allow", v->header.start, v->header.len);
  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", v->value.start,
                               v->value.end - v->value.start);

  // Second value (ACK)
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("ACK", v->value.start,
                               v->value.end - v->value.start);

  // No third value
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NULL(v);
}

void test_value_iterator_multiple_headers(void) {
  struct cmsc_ValueIterator iter;
  struct cmsc_ValueLine value = {0};

  cme_error_t err = cmsc_value_iterator_init(test_buf, strlen(test_buf), &iter);
  TEST_ASSERT_NULL(err);

  struct cmsc_ValueLine *v;

  // First header: Via
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("Via", v->header.start, v->header.len);
  TEST_ASSERT_EQUAL_STRING_LEN("host1;branch=xyz", v->value.start,
                               v->value.end - v->value.start);

  // Second value: host2
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("Via", v->header.start, v->header.len);
  TEST_ASSERT_EQUAL_STRING_LEN("host2;branch=abc", v->value.start,
                               v->value.end - v->value.start);

  // New header: Allow
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("Allow", v->header.start, v->header.len);
  TEST_ASSERT_EQUAL_STRING_LEN("INVITE", v->value.start,
                               v->value.end - v->value.start);

  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("ACK", v->value.start,
                               v->value.end - v->value.start);

  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_EQUAL_STRING_LEN("BYE", v->value.start,
                               v->value.end - v->value.start);

  // Next line is malformed: should return NULL
  v = cmsc_value_iterator_next(&iter, &value);
  TEST_ASSERT_NULL(v);
}

void test_value_iterator_null_args(void) {
  struct cmsc_ValueIterator iter;
  struct cmsc_ValueLine value;

  // NULL buf
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_init(NULL, 10, &iter));

  // NULL iterator
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_init("buf", 4, NULL));

  // NULL value input
  TEST_ASSERT_NULL(cmsc_value_iterator_next(&iter, NULL));

  // NULL iterator input
  TEST_ASSERT_NULL(cmsc_value_iterator_next(NULL, &value));
}

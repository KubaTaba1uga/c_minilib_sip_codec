#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/string.h"

/* static const char *example_line = */
/*     "From: sip:user@example.com;tag=123;ttl=70\r\n"; */

void test_args_iterator_null_inputs(void) {
  struct cmsc_ArgsIterator args_iter;
  struct cmsc_ArgsLine args;

  TEST_ASSERT_NULL(cmsc_args_iterator_next(NULL, NULL));
  TEST_ASSERT_NULL(cmsc_args_iterator_next(&args, NULL));
  TEST_ASSERT_NULL(cmsc_args_iterator_next(NULL, &args_iter));
}

void test_args_iterator_init_null(void) {
  struct cmsc_ArgsIterator args_iter;
  TEST_ASSERT_NOT_NULL(cmsc_args_iterator_init(NULL, &args_iter));
  struct cmsc_ValueLine val = {0};
  TEST_ASSERT_NOT_NULL(cmsc_args_iterator_init(&val, NULL));
}

void test_args_iterator_valid_simple_uri_and_param(void) {
  const char *line = "From: sip:user@example.com;tag=123\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};

  cme_error_t err = cmsc_value_iterator_init(line, strlen(line), &viter);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&viter, &vline));

  struct cmsc_ArgsIterator args_iter;
  TEST_ASSERT_NULL(cmsc_args_iterator_init(&vline, &args_iter));

  struct cmsc_ArgsLine args = {0};
  struct cmsc_ArgsLine *a;

  // URI
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", a->value.start,
                               a->value.len);

  // tag param
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("tag", a->arg_key.start,
                               a->arg_key.end - a->arg_key.start);
  TEST_ASSERT_EQUAL_STRING_LEN("123", a->arg_value.start,
                               a->arg_value.end - a->arg_value.start);

  // done
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NULL(a);
}

void test_args_iterator_multiple_params(void) {
  const char *line = "Via: sip:host;branch=abc;ttl=42\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};

  TEST_ASSERT_NULL(cmsc_value_iterator_init(line, strlen(line), &viter));
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&viter, &vline));

  struct cmsc_ArgsIterator args_iter;
  TEST_ASSERT_NULL(cmsc_args_iterator_init(&vline, &args_iter));

  struct cmsc_ArgsLine args = {0};
  struct cmsc_ArgsLine *a;

  // value
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:host", a->value.start, a->value.len);

  // branch=abc
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("branch", a->arg_key.start,
                               a->arg_key.end - a->arg_key.start);
  TEST_ASSERT_EQUAL_STRING_LEN("abc", a->arg_value.start,
                               a->arg_value.end - a->arg_value.start);

  // ttl=42
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("ttl", a->arg_key.start,
                               a->arg_key.end - a->arg_key.start);
  TEST_ASSERT_EQUAL_STRING_LEN("42", a->arg_value.start,
                               a->arg_value.end - a->arg_value.start);

  // end
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NULL(a);
}

void test_args_iterator_malformed_missing_equal_fails(void) {
  const char *line = "From: sip:user@example.com;badparam\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};

  TEST_ASSERT_NULL(cmsc_value_iterator_init(line, strlen(line), &viter));
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&viter, &vline));

  struct cmsc_ArgsIterator args_iter;
  TEST_ASSERT_NULL(cmsc_args_iterator_init(&vline, &args_iter));

  struct cmsc_ArgsLine args = {0};
  struct cmsc_ArgsLine *a;

  // URI
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NOT_NULL(a);

  // malformed param
  a = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_NULL(a);
}

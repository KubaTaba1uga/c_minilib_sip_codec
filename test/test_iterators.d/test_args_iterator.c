#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/string.h"

void test_args_iterator_null_inputs(void) {
  struct cmsc_ArgsIterator args_iter;
  struct cmsc_ArgsLine args;

  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE,
                    cmsc_args_iterator_next(NULL, NULL));
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE,
                    cmsc_args_iterator_next(&args, NULL));
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE,
                    cmsc_args_iterator_next(NULL, &args_iter));
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

  // URI value
  enum cmsc_ArgsNextResults r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_VALUE, r);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", args.value.start,
                               args.value.len);

  // tag=123
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_ARG, r);
  TEST_ASSERT_EQUAL_STRING_LEN("tag", args.arg_key.start, args.arg_key.len);
  TEST_ASSERT_EQUAL_STRING_LEN("123", args.arg_value.start, args.arg_value.len);

  // done
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE, r);
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

  // value
  enum cmsc_ArgsNextResults r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_VALUE, r);
  TEST_ASSERT_EQUAL_STRING_LEN("sip:host", args.value.start, args.value.len);

  // branch=abc
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_ARG, r);
  TEST_ASSERT_EQUAL_STRING_LEN("branch", args.arg_key.start, args.arg_key.len);
  TEST_ASSERT_EQUAL_STRING_LEN("abc", args.arg_value.start, args.arg_value.len);

  // ttl=42
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_ARG, r);
  TEST_ASSERT_EQUAL_STRING_LEN("ttl", args.arg_key.start, args.arg_key.len);
  TEST_ASSERT_EQUAL_STRING_LEN("42", args.arg_value.start, args.arg_value.len);

  // done
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE, r);
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

  // value
  enum cmsc_ArgsNextResults r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_VALUE, r);

  // badparam → no '='
  r = cmsc_args_iterator_next(&args, &args_iter);
  TEST_ASSERT_EQUAL(cmsc_ArgsNextResults_NONE, r);
}

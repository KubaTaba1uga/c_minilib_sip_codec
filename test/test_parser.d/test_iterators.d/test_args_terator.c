#include <string.h>
#include <unity.h>

#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"

void test_argsiter_init_null_fails(void) {
  cme_error_t err = cmsc_argsiter_init(NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_argsiter_init_valid_succeeds(void) {
  struct cmsc_ArgsIterator argsiter;
  TEST_ASSERT_NULL(cmsc_argsiter_init(&argsiter));
  TEST_ASSERT_NULL(argsiter.value);
  TEST_ASSERT_EQUAL_UINT32(0, argsiter.value_len);
}

void test_argsiter_single_value_no_params(void) {
  const char *input = "sip:user@example.com";
  struct cmsc_ValueIterator viter = {
      .value_start = input,
      .value_end = input + strlen(input),
  };

  struct cmsc_ArgsIterator argsiter;
  TEST_ASSERT_NULL(cmsc_argsiter_init(&argsiter));

  bool has_arg = cmsc_argsiter_next(&viter, &argsiter);
  TEST_ASSERT_TRUE(has_arg);

  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", argsiter.value,
                               argsiter.value_len);
  TEST_ASSERT_NULL(argsiter.args_value);
  TEST_ASSERT_EQUAL_UINT32(0, argsiter.args_value_len);
}

void test_argsiter_single_param(void) {
  const char *input = "sip:user@example.com;tag=123";
  struct cmsc_ValueIterator viter = {
      .value_start = input,
      .value_end = input + strlen(input),
  };

  struct cmsc_ArgsIterator argsiter;
  TEST_ASSERT_NULL(cmsc_argsiter_init(&argsiter));

  // Initial value
  TEST_ASSERT_TRUE(cmsc_argsiter_next(&viter, &argsiter));
  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", argsiter.value,
                               argsiter.value_len);

  // First param
  TEST_ASSERT_EQUAL_STRING_LEN("tag", argsiter.args_header,
                               argsiter.args_header_len);
  TEST_ASSERT_EQUAL_STRING_LEN("123", argsiter.args_value,
                               argsiter.args_value_len);

  // No more
  TEST_ASSERT_FALSE(cmsc_argsiter_next(&viter, &argsiter));
}

void test_argsiter_multiple_params(void) {
  const char *input = "sip:user@example.com;tag=abc;ttl=70";
  struct cmsc_ValueIterator viter = {
      .value_start = input,
      .value_end = input + strlen(input),
  };

  struct cmsc_ArgsIterator argsiter;
  TEST_ASSERT_NULL(cmsc_argsiter_init(&argsiter));

  // URI
  TEST_ASSERT_TRUE(cmsc_argsiter_next(&viter, &argsiter));
  TEST_ASSERT_EQUAL_STRING_LEN("sip:user@example.com", argsiter.value,
                               argsiter.value_len);

  // First param
  TEST_ASSERT_EQUAL_STRING_LEN("tag", argsiter.args_header,
                               argsiter.args_header_len);
  TEST_ASSERT_EQUAL_STRING_LEN("abc", argsiter.args_value,
                               argsiter.args_value_len);

  // Second param
  TEST_ASSERT_TRUE(cmsc_argsiter_next(&viter, &argsiter));
  TEST_ASSERT_EQUAL_STRING_LEN("ttl", argsiter.args_header,
                               argsiter.args_header_len);
  TEST_ASSERT_EQUAL_STRING_LEN("70", argsiter.args_value,
                               argsiter.args_value_len);

  // Done
  TEST_ASSERT_FALSE(cmsc_argsiter_next(&viter, &argsiter));
}

void test_argsiter_malformed_missing_equal(void) {
  const char *input = "sip:user@example.com;noequalparam";
  struct cmsc_ValueIterator viter = {
      .value_start = input,
      .value_end = input + strlen(input),
  };

  struct cmsc_ArgsIterator argsiter;
  TEST_ASSERT_NULL(cmsc_argsiter_init(&argsiter));

  TEST_ASSERT_FALSE(
      cmsc_argsiter_next(&viter, &argsiter)); // should fail on malformed param
}

void test_argsiter_null_inputs_return_false(void) {
  struct cmsc_ValueIterator *v = NULL;
  struct cmsc_ArgsIterator a;

  cmsc_argsiter_init(&a);
  TEST_ASSERT_FALSE(cmsc_argsiter_next(NULL, NULL));
  TEST_ASSERT_FALSE(cmsc_argsiter_next(v, &a));
  TEST_ASSERT_FALSE(cmsc_argsiter_next((struct cmsc_ValueIterator[]){0}, NULL));
}

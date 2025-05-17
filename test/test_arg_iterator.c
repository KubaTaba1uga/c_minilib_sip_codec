#include <stdlib.h>
#include <string.h>

#include "utils/tag_iterator.h"
#include <unity_wrapper.h>

void setUp(void) {}
void tearDown(void) {}

void test_iterator_init_null(void) {
  cme_error_t err = cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = "abc", .len = 3}, NULL);
  TEST_ASSERT_NOT_EQUAL(0, err);
}

void test_iterator_init_basic(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "foo";
  cme_error_t err =
      cmsc_arg_iterator_init((struct cmsc_String){.buf = buf, .len = 3}, &it);
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_EQUAL_PTR(buf, it.buf.buf);
  TEST_ASSERT_EQUAL(3, it.buf.len);
}

void test_next_only_value(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "value";
  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  // No explicit assignment to it.value
  enum cmsc_ArgNextResults res = cmsc_arg_iterator_next(&it);

  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);
  // Should not split into arg_key/arg_value
  TEST_ASSERT_NULL(it.arg_key.buf);
  TEST_ASSERT_NULL(it.arg_value.buf);

  // The .value struct is empty in this API, so check the buffer directly
  TEST_ASSERT_EQUAL_STRING(buf, it.value.buf);
  TEST_ASSERT_EQUAL(strlen(buf), it.value.len);
}

void test_next_key_value_pair(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "key=value";
  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  enum cmsc_ArgNextResults res = cmsc_arg_iterator_next(&it);

  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);

  TEST_ASSERT_NULL(it.arg_key.buf);
  TEST_ASSERT_NULL(it.arg_value.buf);
  TEST_ASSERT_EQUAL_STRING(buf, it.value.buf);
  TEST_ASSERT_EQUAL(strlen(buf), it.value.len);
}

void test_next_arg_semicolon(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "key=value;param=123";
  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  // First call
  enum cmsc_ArgNextResults res1 = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res1);
  TEST_ASSERT_NULL(it.arg_key.buf);
  TEST_ASSERT_NULL(it.arg_value.buf);

  // Second call: try next
  enum cmsc_ArgNextResults res2 = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_ARG, res2);
}

void test_multiple_arguments(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "host;ttl=70;received=192.0.2.1";
  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  enum cmsc_ArgNextResults res;

  // 1. VALUE -> host
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);
  TEST_ASSERT_EQUAL_STRING_LEN("host", it.value.buf, it.value.len);

  // 2. ARG -> ttl=70
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_ARG, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("ttl", it.arg_key.buf, it.arg_key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("70", it.arg_value.buf, it.arg_value.len);

  // 3. ARG -> received=192.0.2.1
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_ARG, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("received", it.arg_key.buf, it.arg_key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("192.0.2.1", it.arg_value.buf,
                                 it.arg_value.len);

  // 4. END
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_NONE, res);
}

void test_argument_without_value(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "foo;no_value_key;";
  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  enum cmsc_ArgNextResults res;

  // 1. VALUE -> foo
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("foo", it.value.buf, it.value.len);

  // 2. ARG -> no_value_key
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_NONE, res);
}

void test_comma_separated_values(void) {
  struct cmsc_ArgIterator it;
  const char *buf = "SIP/2.0/UDP a.example.com;branch=z9hG1,"
                    "SIP/2.0/UDP b.example.com;branch=z9hG2";

  cmsc_arg_iterator_init(
      (struct cmsc_String){.buf = buf, .len = (uint32_t)strlen(buf)}, &it);

  enum cmsc_ArgNextResults res;

  // First value: SIP/2.0/UDP a.example.com
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0/UDP a.example.com", it.value.buf,
                                 it.value.len);

  // First param: branch=z9hG1
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_ARG, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("branch", it.arg_key.buf, it.arg_key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z9hG1", it.arg_value.buf, it.arg_value.len);

  // Second value: SIP/2.0/UDP b.example.com
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_VALUE, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("SIP/2.0/UDP b.example.com", it.value.buf,
                                 it.value.len);

  // Second param: branch=z9hG2
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_ARG, res);
  MYTEST_ASSERT_EQUAL_STRING_LEN("branch", it.arg_key.buf, it.arg_key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("z9hG2", it.arg_value.buf, it.arg_value.len);

  // End
  res = cmsc_arg_iterator_next(&it);
  TEST_ASSERT_EQUAL(cmsc_ArgNextResults_NONE, res);
}

#include <unity.h>

#define MYTEST_ASSERT_EQUAL_STRING_LEN(expected, actual, actual_len)           \
  TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, strlen(expected));            \
  TEST_ASSERT_EQUAL(strlen(expected), actual_len);

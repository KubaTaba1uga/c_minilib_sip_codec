#include <string.h>
#include <unity.h>

#include "utils/string.h"

void test_empty_needle_returns_haystack(void) {
  const char *hay = "abcde";
  const char *res = cmsc_strnstr(hay, "", 5);
  TEST_ASSERT_EQUAL_PTR(hay, res);
}

void test_not_found(void) {
  const char *hay = "abcde";
  const char *res = cmsc_strnstr(hay, "xyz", 5);
  TEST_ASSERT_NULL(res);
}

void test_found_simple(void) {
  const char *hay = "abcde";
  const char *res = cmsc_strnstr(hay, "cd", 5);
  TEST_ASSERT_EQUAL_STRING("cde", res);
}

void test_partial_match_at_end(void) {
  const char *hay = "abcde";
  const char *res = cmsc_strnstr(hay, "de", 5);
  TEST_ASSERT_EQUAL_STRING("de", res);
}

void test_match_stops_at_length_limit(void) {
  const char *hay = "abcde";
  const char *res = cmsc_strnstr(hay, "de", 4); // only up to 'd'
  TEST_ASSERT_NULL(res);
}

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/args_iterator.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "utils/string.h"

void prepare_value(const char *line, struct cmsc_Value *value) {
  struct cmsc_ValueIterator value_iter;
  struct cmsc_LineIterator line_iter;
  struct cmsc_Line line;

  // Init line iterator
  TEST_ASSERT_NULL(cmsc_line_iterator_init(line, strlen(line), &line_iter));
  TEST_ASSERT_TRUE(cmsc_line_iterator_next(&line_iter, &line));

  // Init value iterator from single line
  TEST_ASSERT_NULL(cmsc_value_iterator_init(line.start, line.len, &value_iter));
  TEST_ASSERT_TRUE(cmsc_value_iterator_next(&value_iter, value));
}

void test_args_iterator_single_argument(void) {
  struct cmsc_Value value = {0};
  prepare_value("X-Test: param=value\r\n", &value);

  struct cmsc_ArgsIterator iter;
  struct cmsc_Args arg = {0};

  TEST_ASSERT_NULL(cmsc_args_iterator_init(&value, &iter));
  struct cmsc_Args *a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("param=value", a->value.start, a->value.len);
}

void test_args_iterator_multiple_arguments(void) {
  struct cmsc_Value value = {0};
  prepare_value("Via: host;branch=abc;ttl=50\r\n", &value);

  struct cmsc_ArgsIterator iter;
  struct cmsc_Args arg = {0};

  TEST_ASSERT_NULL(cmsc_args_iterator_init(&value, &iter));

  struct cmsc_Args *a;

  a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("host", a->value.start, a->value.len);

  a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("branch=abc", a->value.start, a->value.len);

  a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_EQUAL_STRING_LEN("ttl=50", a->value.start, a->value.len);

  a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NULL(a);
}

void test_args_iterator_empty_value(void) {
  struct cmsc_Value value = {0};
  prepare_value("Empty: \r\n", &value);

  struct cmsc_ArgsIterator iter;
  struct cmsc_Args arg = {0};

  TEST_ASSERT_NULL(cmsc_args_iterator_init(&value, &iter));
  struct cmsc_Args *a = cmsc_args_iterator_next(&arg, &iter);
  TEST_ASSERT_NULL(a);
}

void test_args_iterator_null_arguments(void) {
  struct cmsc_ArgsIterator iter;
  struct cmsc_Args arg;

  TEST_ASSERT_NULL(cmsc_args_iterator_next(NULL, &iter));
  TEST_ASSERT_NULL(cmsc_args_iterator_next(&arg, NULL));
  TEST_ASSERT_NOT_NULL(cmsc_args_iterator_init(NULL, &iter));
  TEST_ASSERT_NOT_NULL(cmsc_args_iterator_init((void *)1, NULL));
}

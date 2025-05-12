#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "parser/iterator/line_iterator.h"

static const char *multi_line = "Line-One\r\n"
                                "Line-Two\r\n"
                                "Line-Three\r\n";

void test_line_iterator_valid_lines(void) {
  struct cmsc_LineIterator iter;
  struct cmsc_Line line;

  cme_error_t err =
      cmsc_line_iterator_init(multi_line, strlen(multi_line), &iter);
  TEST_ASSERT_NULL(err);

  struct cmsc_Line *l1 = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NOT_NULL(l1);
  TEST_ASSERT_EQUAL_STRING_LEN("Line-One", line.start, line.len);

  struct cmsc_Line *l2 = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NOT_NULL(l2);
  TEST_ASSERT_EQUAL_STRING_LEN("Line-Two", line.start, line.len);

  struct cmsc_Line *l3 = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NOT_NULL(l3);
  TEST_ASSERT_EQUAL_STRING_LEN("Line-Three", line.start, line.len);

  // No more lines
  struct cmsc_Line *l4 = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NULL(l4);
}

void test_line_iterator_empty_line(void) {
  const char *text = "\r\n";
  struct cmsc_LineIterator iter;
  struct cmsc_Line line;

  TEST_ASSERT_NULL(cmsc_line_iterator_init(text, strlen(text), &iter));

  struct cmsc_Line *l = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NOT_NULL(l);
  TEST_ASSERT_EQUAL(0, line.len);
}

void test_line_iterator_no_crlf(void) {
  const char *text = "NoCRLFHere";
  struct cmsc_LineIterator iter;
  struct cmsc_Line line;

  TEST_ASSERT_NULL(cmsc_line_iterator_init(text, strlen(text), &iter));
  struct cmsc_Line *l = cmsc_line_iterator_next(&iter, &line);
  TEST_ASSERT_NULL(l); // no \r\n → NULL
}

void test_line_iterator_invalid_args(void) {
  struct cmsc_LineIterator iter;
  struct cmsc_Line line;

  // NULL input
  TEST_ASSERT_NOT_NULL(cmsc_line_iterator_init(NULL, 10, &iter));
  TEST_ASSERT_NOT_NULL(cmsc_line_iterator_init(multi_line, 0, &iter));
  TEST_ASSERT_NOT_NULL(
      cmsc_line_iterator_init(multi_line, strlen(multi_line), NULL));

  // NULL line struct
  TEST_ASSERT_NULL(cmsc_line_iterator_next(&iter, NULL));

  // NULL iterator
  TEST_ASSERT_NULL(cmsc_line_iterator_next(NULL, &line));
}

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_allow.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"

static cmsc_sipmsg_t msg = NULL;


void setUp(void) {
  cme_init();
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&msg));


}

void tearDown(void) {
  cmsc_sipmsg_destroy(&msg);
  msg = NULL;


  cme_destroy();
}



static void prepare_value_lines(const char *line,
                                struct cmsc_ValueIterator *viter) {
  TEST_ASSERT_NULL(cmsc_value_iterator_init(line, strlen(line), viter));
}

void test_parse_allow_single_method(void) {
  const char *line = "Allow: INVITE\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};
  prepare_value_lines(line, &viter);
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&viter, &vline));

  cme_error_t err = cmsc_parser_parse_allow(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ALLOW));
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_INVITE);
}

void test_parse_allow_multiple_methods_comma_separated(void) {
  const char *line = "Allow: INVITE, ACK, BYE\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};
  prepare_value_lines(line, &viter);

  while (cmsc_value_iterator_next(&viter, &vline)) {
    TEST_ASSERT_NULL(cmsc_parser_parse_allow(&vline, &msg));
  }

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ALLOW));
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_INVITE);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_ACK);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_BYE);
}

void test_parse_allow_with_unknown_method_ignored(void) {
  const char *line = "Allow: UNKNOWN, INVITE, OPTIONS\r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};
  prepare_value_lines(line, &viter);

  while (cmsc_value_iterator_next(&viter, &vline)) {
    TEST_ASSERT_NULL(cmsc_parser_parse_allow(&vline, &msg));
  }

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ALLOW));
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_INVITE);
  TEST_ASSERT_TRUE(msg->allow_mask & cmsc_SupportedMessages_OPTIONS);
  // UNKNOWN is ignored gracefully
}

void test_parse_allow_empty_value(void) {
  const char *line = "Allow: \r\n";
  struct cmsc_ValueIterator viter;
  struct cmsc_ValueLine vline = {0};
  prepare_value_lines(line, &viter);

  while (cmsc_value_iterator_next(&viter, &vline)) {
    TEST_ASSERT_NULL(cmsc_parser_parse_allow(&vline, &msg));
  }

  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ALLOW));
  TEST_ASSERT_EQUAL(0, msg->allow_mask);
}

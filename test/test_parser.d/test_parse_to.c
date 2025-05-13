#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_to.h"
#include "sipmsg/sipmsg.h"

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

static void prepare_value_line(struct cmsc_ValueLine *vline,
                               const char *header_line) {
  struct cmsc_ValueIterator viter;
  TEST_ASSERT_NULL(
      cmsc_value_iterator_init(header_line, strlen(header_line), &viter));
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&viter, vline));
}

void test_parse_to_with_tag(void) {
  const char *line = "To: <sip:alice@example.com>;tag=abc123\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:alice@example.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("abc123", msg->to.tag);
}

void test_parse_to_without_tag(void) {
  const char *line = "To: <sip:bob@example.com>\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->to.uri);
  TEST_ASSERT_NULL(msg->to.tag);
}

void test_parse_to_with_multiple_args(void) {
  const char *line = "To: <sip:bob@example.com>;tag=xyz;ttl=60\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("xyz", msg->to.tag);
}

void test_parse_to_with_display_name_ignored(void) {
  const char *line = "To: \"Alice\" <sip:alice@example.com>;tag=abc\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:alice@example.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("abc", msg->to.tag);
  // Display name is not handled in new implementation
}

void test_parse_to_without_display_name(void) {
  const char *line = "To: <sip:carol@example.com>;tag=42\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:carol@example.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("42", msg->to.tag);
}

void test_parse_to_missing_uri(void) {
  const char *line = "To: ;tag=missinguri\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_to(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_FALSE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_NULL(msg->to.uri);
  TEST_ASSERT_NULL(msg->to.tag);
}

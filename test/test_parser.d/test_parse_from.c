#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_from.h" // <- You need this file to exist
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

static void prepare_value_iterator(struct cmsc_ValueIterator *viter,
                                   const char *line, const char *value_substr) {
  size_t header_len = strlen("From");
  const char *colon = strchr(line, ':');
  TEST_ASSERT_NOT_NULL(colon);

  viter->header_start = line;
  viter->header_len = header_len;
  viter->value_start = colon + 1;
  while (isspace(*viter->value_start))
    viter->value_start++;
  viter->value_end = line + strlen(line);
}

void test_parse_from_with_tag(void) {
  const char *line = "From: <sip:alice@example.com>;tag=abc123";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line, "<sip:alice@example.com>;tag=abc123");

  cme_error_t err =
      cmsc_parser_parse_from(&viter, msg); // <- adjust parser here
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:alice@example.com>", msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("abc123", msg->from.tag);
}

void test_parse_from_without_tag(void) {
  const char *line = "From: <sip:bob@example.com>";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line, "<sip:bob@example.com>");

  cme_error_t err = cmsc_parser_parse_from(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->from.uri);
  TEST_ASSERT_NULL(msg->from.tag);
}

void test_parse_from_with_multiple_args(void) {
  const char *line = "From: <sip:bob@example.com>;tag=xyz;ttl=60";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line, "<sip:bob@example.com>;tag=xyz;ttl=60");

  cme_error_t err = cmsc_parser_parse_from(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("xyz", msg->from.tag);
}

void test_parse_from_with_display_name_ignored(void) {
  const char *line = "From: \"Alice\" <sip:alice@example.com>;tag=abc";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line,
                         "\"Alice\" <sip:alice@example.com>;tag=abc");

  cme_error_t err = cmsc_parser_parse_from(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:alice@example.com>", msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("\"Alice\"", msg->from.display_name);
  TEST_ASSERT_EQUAL_STRING("abc", msg->from.tag);
}

void test_parse_from_without_display_name(void) {
  const char *line = "From: <sip:carol@example.com>;tag=42";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line, "<sip:carol@example.com>;tag=42");

  cme_error_t err = cmsc_parser_parse_from(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_EQUAL_STRING("<sip:carol@example.com>", msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("42", msg->from.tag);
  TEST_ASSERT_NULL(msg->from.display_name);
}

void test_parse_from_missing_uri(void) {
  const char *line = "From: ;tag=missinguri";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line, "tag=missinguri");

  cme_error_t err = cmsc_parser_parse_from(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_NULL(msg->from.uri);
  TEST_ASSERT_NULL(msg->from.tag);
}

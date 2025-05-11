#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/supported_messages.h"

static cmsc_parser_t parser = NULL;

void setUp(void) {
  cme_init();
  cmsc_parser_create(&parser);
  cmsc_supmsg_init();
}

void tearDown(void) {
  cmsc_supmsg_destroy();
  cmsc_parser_destroy(&parser);
  parser = NULL;
  cme_destroy();
}

static struct cmsc_HeaderIterator
prepare_parser_with_invite_and_headers(const char *headers) {
  const char *first_line = "INVITE sip:bob@example.com SIP/2.0\r\n";
  size_t full_len = strlen(first_line) + strlen(headers);
  char *packet = malloc(full_len + 1);
  strcpy(packet, first_line);
  strcat(packet, headers);

  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)packet,
      .buf_len = (uint32_t)strlen(packet),
  };

  TEST_ASSERT_NULL(cmsc_dynbuf_put(view.buf_len, (char *)view.buf,
                                   (void **)&parser, sizeof(struct cmsc_Parser),
                                   &parser->content));

  struct cmsc_HeaderIterator header_iter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&header_iter));
  bool is_next = false;

  TEST_ASSERT_NULL(cmsc_parser_parse_msgempty(&header_iter, parser, &is_next));
  TEST_ASSERT_TRUE(is_next);

  free(packet);

  return header_iter;
}

void test_parse_headers_with_to_header_succeeds(void) {
  struct cmsc_HeaderIterator header_iter =
      prepare_parser_with_invite_and_headers(
          "To: <sip:bob@example.com>;tag=abc123\r\n");

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_headers(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->to.uri);
  TEST_ASSERT_NOT_NULL(msg->to.tag);
  TEST_ASSERT_EQUAL_STRING("abc123", msg->to.tag);
}

void test_parse_headers_skips_unrecognized_field(void) {
  struct cmsc_HeaderIterator header_iter =
      prepare_parser_with_invite_and_headers(
          "X-Custom: something\r\n"
          "To: <sip:bob@example.com>;tag=abc123\r\n");

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_headers(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
}

void test_parse_headers_with_no_matching_fields_aborts(void) {
  prepare_parser_with_invite_and_headers("Unknown-Field: somevalue\r\n");

  struct cmsc_HeaderIterator header_iter;
  TEST_ASSERT_NULL(cmsc_headeriter_init(&header_iter));

  bool is_next = false;
  cme_error_t err = cmsc_parser_parse_headers(&header_iter, parser, &is_next);
  TEST_ASSERT_NULL(err); // should silently stop without error

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_FALSE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
}

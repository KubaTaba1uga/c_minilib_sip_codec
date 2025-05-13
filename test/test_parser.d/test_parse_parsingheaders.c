#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"
#include "supported_messages/supported_messages.h"
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static cmsc_sipmsg_t msg = NULL;

void setUp(void) {
  cme_init();
  cmsc_supmsg_init();
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&msg));
}

void tearDown(void) {
  cmsc_sipmsg_destroy(&msg);
  msg = NULL;
  cmsc_supmsg_destroy();
  cme_destroy();
}

static void prepare_value_line(struct cmsc_ValueLine *vline, const char *line) {
  struct cmsc_ValueIterator iter;
  TEST_ASSERT_NULL(cmsc_value_iterator_init(line, strlen(line), &iter));
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&iter, vline));
}

void test_parse_sip_header_to_success(void) {
  const char *line = "To: <sip:bob@example.com>;tag=abc\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  // Set supmsg manually to allow scheme resolution
  msg->supmsg = cmsc_SupportedMessages_INVITE;

  cmsc_sipmsg_t *msg_ptr = &msg;
  TEST_ASSERT_NOT_NULL(cmsc_parser_parse_sip_header(&vline, msg_ptr));

  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_TO));
  TEST_ASSERT_EQUAL_STRING("<sip:bob@example.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("abc", msg->to.tag);
}

void test_parse_sip_header_unknown_header_returns_null(void) {
  const char *line = "X-Unknown: value\r\n";
  struct cmsc_ValueLine vline;
  prepare_value_line(&vline, line);

  msg->supmsg = cmsc_SupportedMessages_INVITE;

  cmsc_sipmsg_t *msg_ptr = &msg;

  // We are just skipping unrecognized fields
  TEST_ASSERT_NOT_NULL(cmsc_parser_parse_sip_header(&vline, msg_ptr));
}

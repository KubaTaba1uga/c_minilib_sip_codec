#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/parser.h"
#include "sipmsg/sipmsg.h"

static cmsc_parser_t parser = NULL;

void setUp(void) {
  cme_init();
  cmsc_parser_create(&parser);
}

void tearDown(void) {
  cmsc_parser_destroy(&parser);
  parser = NULL;
  cme_destroy();
}

void test_parse_valid_invite_line(void) {
  const char *line = "INVITE sip:bob@example.com SIP/2.0\r\n";
  struct cmsc_CharBufferView view = {
      .buf = (const uint8_t *)line,
      .buf_len = (uint32_t)strlen(line),
  };

  cme_error_t err = cmsc_parser_feed_data(view, &parser);
  TEST_ASSERT_NULL(err);

  cmsc_sipmsg_t msg = parser->msg;
  TEST_ASSERT_NOT_NULL(msg);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_IS_REQUEST));
  TEST_ASSERT_TRUE(msg->is_request);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG));
  TEST_ASSERT_EQUAL(cmsc_SupportedMessages_INVITE, msg->supmsg);
}

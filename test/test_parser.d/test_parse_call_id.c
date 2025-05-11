#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_call_id.h"
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
                                   const char *line) {
  const char *colon = strchr(line, ':');
  TEST_ASSERT_NOT_NULL(colon);

  viter->header_start = line;
  viter->header_len = (uint32_t)(colon - line);
  viter->value_start = colon + 1;

  while (isspace(*viter->value_start)) {
    viter->value_start++;
  }

  viter->value_end = line + strlen(line);
}

void test_parse_call_id_simple(void) {
  const char *line = "Call-ID: abc123";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_call_id(&viter, msg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->call_id);
  TEST_ASSERT_EQUAL_STRING("abc123", msg->call_id);
}

void test_parse_call_id_with_whitespace(void) {
  const char *line = "Call-ID:     xYz-42@host.com";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_call_id(&viter, msg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL_STRING("xYz-42@host.com", msg->call_id);
}

void test_parse_call_id_empty_value(void) {
  const char *line = "Call-ID:    ";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_call_id(&viter, msg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(msg->call_id);
}

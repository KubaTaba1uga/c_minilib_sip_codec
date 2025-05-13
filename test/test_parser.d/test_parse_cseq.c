#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_cseq.h"
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

static void prepare_value_line(struct cmsc_ValueLine *vline, const char *line) {
  struct cmsc_ValueIterator iter;
  TEST_ASSERT_NULL(cmsc_value_iterator_init(line, strlen(line), &iter));
  TEST_ASSERT_NOT_NULL(cmsc_value_iterator_next(&iter, vline));
}

void test_parse_cseq_valid_invite(void) {
  const char *line = "CSeq: 123 INVITE\r\n";
  struct cmsc_ValueLine vline = {0};
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_cseq(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(123, msg->cseq.seq_number);
  TEST_ASSERT_NOT_NULL(msg->cseq.method);
  TEST_ASSERT_EQUAL_STRING("INVITE", msg->cseq.method);
}

void test_parse_cseq_valid_ack_with_spaces(void) {
  const char *line = "CSeq:   456     ACK\r\n";
  struct cmsc_ValueLine vline = {0};
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_cseq(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(456, msg->cseq.seq_number);
  TEST_ASSERT_EQUAL_STRING("ACK", msg->cseq.method);
}

void test_parse_cseq_missing_method(void) {
  const char *line = "CSeq: 789\r\n";
  struct cmsc_ValueLine vline = {0};
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_cseq(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(789, msg->cseq.seq_number);
  TEST_ASSERT_NULL(msg->cseq.method);
}

void test_parse_cseq_missing_number(void) {
  const char *line = "CSeq: INVITE\r\n";
  struct cmsc_ValueLine vline = {0};
  prepare_value_line(&vline, line);

  cme_error_t err = cmsc_parser_parse_cseq(&vline, &msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(0, msg->cseq.seq_number); // atoi fails
  TEST_ASSERT_EQUAL_STRING("INVITE", msg->cseq.method);
}

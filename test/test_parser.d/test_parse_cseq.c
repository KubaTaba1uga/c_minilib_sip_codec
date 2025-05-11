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

void test_parse_cseq_valid_invite(void) {
  const char *line = "CSeq: 123 INVITE";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_cseq(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(123, msg->cseq.seq_number);
  TEST_ASSERT_NOT_NULL(msg->cseq.method);
  TEST_ASSERT_EQUAL_STRING("INVITE", msg->cseq.method);
}

void test_parse_cseq_valid_ack_with_spaces(void) {
  const char *line = "CSeq:   456     ACK";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_cseq(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(456, msg->cseq.seq_number);
  TEST_ASSERT_EQUAL_STRING("ACK", msg->cseq.method);
}

void test_parse_cseq_missing_method(void) {
  const char *line = "CSeq: 789";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_cseq(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(789, msg->cseq.seq_number);
  TEST_ASSERT_NULL(msg->cseq.method);
}

void test_parse_cseq_missing_number(void) {
  const char *line = "CSeq: INVITE";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_cseq(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL_UINT32(0, msg->cseq.seq_number); // atoi of method fails
  TEST_ASSERT_EQUAL_STRING("INVITE", msg->cseq.method);
}

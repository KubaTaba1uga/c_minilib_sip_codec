#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_via_l.h"
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

void test_parse_via_basic_udp(void) {
  const char *line = "Via: SIP/2.0/UDP server1.example.com";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_via_l(&viter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_VIA_L));
  struct cmsc_Field_Via *via = STAILQ_FIRST(&msg->via);
  TEST_ASSERT_NOT_NULL(via);
  TEST_ASSERT_EQUAL(cmsc_TransportProtocols_UDP, via->transp_proto);
  TEST_ASSERT_EQUAL_STRING("server1.example.com", via->sent_by);
  TEST_ASSERT_NULL(via->branch);
  TEST_ASSERT_NULL(via->addr);
}

void test_parse_via_with_branch_and_addr(void) {
  const char *line =
      "Via: SIP/2.0/UDP sip.test.org;branch=z9hG4bK-123;addr=10.0.0.1";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_via_l(&viter, msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_Field_Via *via = STAILQ_FIRST(&msg->via);
  TEST_ASSERT_NOT_NULL(via);
  TEST_ASSERT_EQUAL(cmsc_TransportProtocols_UDP, via->transp_proto);
  TEST_ASSERT_EQUAL_STRING("sip.test.org", via->sent_by);
  TEST_ASSERT_EQUAL_STRING("z9hG4bK-123", via->branch);
  TEST_ASSERT_EQUAL_STRING("10.0.0.1", via->addr);
}

void test_parse_via_missing_proto_returns_error(void) {
  const char *line = "Via: somethingwrong";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_via_l(&viter, msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_via_multiple_entries_two_parsed(void) {
  const char *line =
      "Via: SIP/2.0/UDP host1;branch=b1, SIP/2.0/UDP host2;branch=b2";
  struct cmsc_ValueIterator viter;
  prepare_value_iterator(&viter, line);

  cme_error_t err = cmsc_parser_parse_via_l(&viter, msg);
  TEST_ASSERT_NULL(err);

  // First Via entry
  struct cmsc_Field_Via *via1 = STAILQ_FIRST(&msg->via);
  TEST_ASSERT_NOT_NULL(via1);
  TEST_ASSERT_EQUAL(cmsc_TransportProtocols_UDP, via1->transp_proto);
  TEST_ASSERT_EQUAL_STRING("host1", via1->sent_by);
  TEST_ASSERT_EQUAL_STRING("b1", via1->branch);

  // Second Via entry
  struct cmsc_Field_Via *via2 = STAILQ_NEXT(via1, vias_l);
  TEST_ASSERT_NOT_NULL(via2);
  TEST_ASSERT_EQUAL(cmsc_TransportProtocols_UDP, via2->transp_proto);
  TEST_ASSERT_EQUAL_STRING("host2", via2->sent_by);
  TEST_ASSERT_EQUAL_STRING("b2", via2->branch);

  // No more entries
  TEST_ASSERT_NULL(STAILQ_NEXT(via2, vias_l));
}

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_via_l.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"

static cmsc_sipmsg_t msg = NULL;

struct DynBufContainer {
  struct cmsc_DynamicBuffer dynbuf;
};

static struct DynBufContainer *container = NULL;

void setUp(void) {
  cme_init();
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&msg));

  // Allocate a dynamic container for dynbuf
  size_t alloc_size = sizeof(struct DynBufContainer) + 512;
  container = malloc(alloc_size);
  TEST_ASSERT_NOT_NULL(container);
  TEST_ASSERT_NULL(cmsc_dynbuf_init(512, &container->dynbuf));
}

void tearDown(void) {
  cmsc_sipmsg_destroy(&msg);
  msg = NULL;

  free(container);
  container = NULL;

  cme_destroy();
}

static void prepare_iterators(const char *line,
                               struct cmsc_HeaderIterator *header_iter,
                               struct cmsc_ValueIterator *value_iter) {
  TEST_ASSERT_NULL(cmsc_headeriter_init(header_iter));
  TEST_ASSERT_NULL(cmsc_valueiter_init(value_iter));

  cmsc_dynbuf_flush(&container->dynbuf);

  TEST_ASSERT_NULL(cmsc_dynbuf_put(strlen(line), (char *)line,
                                   (void **)&container, sizeof(*container),
                                   &container->dynbuf));

  TEST_ASSERT_TRUE(cmsc_headeriter_next(&container->dynbuf, header_iter));
  TEST_ASSERT_TRUE(cmsc_valueiter_next(header_iter, value_iter));
}

void test_parse_via_basic_udp(void) {
  const char *line = "Via: SIP/2.0/UDP server1.example.com\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_via_l(&header_iter, &value_iter, msg);
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
      "Via: SIP/2.0/UDP sip.test.org;branch=z9hG4bK-123;addr=10.0.0.1\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_via_l(&header_iter, &value_iter, msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_Field_Via *via = STAILQ_FIRST(&msg->via);
  TEST_ASSERT_NOT_NULL(via);
  TEST_ASSERT_EQUAL(cmsc_TransportProtocols_UDP, via->transp_proto);
  TEST_ASSERT_EQUAL_STRING("sip.test.org", via->sent_by);
  TEST_ASSERT_EQUAL_STRING("z9hG4bK-123", via->branch);
  TEST_ASSERT_EQUAL_STRING("10.0.0.1", via->addr);
}

void test_parse_via_missing_proto_returns_error(void) {
  const char *line = "Via: somethingwrong\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_via_l(&header_iter, &value_iter, msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_via_multiple_entries_two_parsed(void) {
  const char *line =
      "Via: SIP/2.0/UDP host1;branch=b1, SIP/2.0/UDP host2;branch=b2\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_via_l(&header_iter, &value_iter, msg);
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


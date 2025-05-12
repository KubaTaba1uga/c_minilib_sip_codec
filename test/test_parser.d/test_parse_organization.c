#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_organization.h"
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
  container = malloc(sizeof(struct DynBufContainer) + 256);
  TEST_ASSERT_NOT_NULL(container);
  TEST_ASSERT_NULL(cmsc_dynbuf_init(256, &container->dynbuf));
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

void test_parse_organization_with_name(void) {
  const char *line = "Organization: OpenAI Communications\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_organization(&value_iter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ORGANIZATION));
  TEST_ASSERT_NOT_NULL(msg->organization);
  TEST_ASSERT_EQUAL_STRING("OpenAI Communications", msg->organization);
}

void test_parse_organization_whitespace_only(void) {
  const char *line = "Organization:    \t\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_organization(&value_iter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ORGANIZATION));
  TEST_ASSERT_NULL(msg->organization);
}

void test_parse_organization_empty(void) {
  const char *line = "Organization:\r\n";
  struct cmsc_HeaderIterator header_iter;
  struct cmsc_ValueIterator value_iter;
  prepare_iterators(line, &header_iter, &value_iter);

  cme_error_t err = cmsc_parser_parse_organization(&value_iter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ORGANIZATION));
  TEST_ASSERT_NULL(msg->organization);
}

void test_parse_organization_value_null(void) {
  struct cmsc_ValueIterator value_iter = {0};
  value_iter.value_start = NULL;
  value_iter.value_end = NULL;

  cme_error_t err = cmsc_parser_parse_organization(&value_iter, msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedFields_ORGANIZATION));
  TEST_ASSERT_NULL(msg->organization);
}

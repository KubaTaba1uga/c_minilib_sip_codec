#include <string.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"

static cmsc_sipmsg_t sipmsg = NULL;

void setUp(void) {
  cme_init();
  sipmsg = NULL;
}

void tearDown(void) {
  if (sipmsg) {
    cmsc_dynbuf_destroy(&sipmsg->content);
    free(sipmsg);
    sipmsg = NULL;
  }
  cme_destroy();
}

void test_create_null_ptr_returns_error(void) {
  cme_error_t err = cmsc_sipmsg_create(NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_create_valid_succeeds(void) {
  cme_error_t err = cmsc_sipmsg_create(&sipmsg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(sipmsg);
}

void test_insert_str_null_input_returns_null(void) {
  const char *result;

  // Null buffer
  result = cmsc_sipmsg_insert_str(4, NULL, &sipmsg);
  TEST_ASSERT_NULL(result);

  // Null sipmsg
  result = cmsc_sipmsg_insert_str(4, "test", NULL);
  TEST_ASSERT_NULL(result);
}

void test_insert_str_valid_appends(void) {
  const char *data = "Hello";
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&sipmsg));
  TEST_ASSERT_NOT_NULL(sipmsg);
  const char *result = cmsc_sipmsg_insert_str(strlen(data), data, &sipmsg);
  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_STRING_LEN(data, result, strlen(data));
}

void test_dump_supported_messages_valid_range(void) {
  TEST_ASSERT_EQUAL_STRING("INVITE", cmsc_dump_supported_messages_string(
                                         cmsc_SupportedMessages_INVITE));

  TEST_ASSERT_EQUAL_STRING("200 OK", cmsc_dump_supported_messages_string(
                                         cmsc_SupportedMessages_200_OK));
}

void test_dump_supported_messages_invalid_range_returns_null(void) {
  TEST_ASSERT_NULL(cmsc_dump_supported_messages_string(0));
  TEST_ASSERT_NULL(
      cmsc_dump_supported_messages_string(cmsc_SupportedMessages_MAX));
}

void test_field_presence_set_and_check(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&sipmsg));

  // Before marking: not present
  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_TO));

  // Mark field present
  cmsc_sipmsg_mark_field_present(sipmsg, cmsc_SupportedFields_TO);

  // Should now be present
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_TO));
}

void test_field_multiple_markings_accumulate(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create(&sipmsg));

  cmsc_sipmsg_mark_field_present(sipmsg, cmsc_SupportedFields_FROM);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_FALSE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_CSEQ));

  cmsc_sipmsg_mark_field_present(sipmsg, cmsc_SupportedFields_CSEQ);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_FROM));
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(sipmsg, cmsc_SupportedFields_CSEQ));
}

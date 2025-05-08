#include <c_minilib_error.h>
#include <stdlib.h>
#include <unity.h>

#include "sip_msg/sip_msg.h"

#define TEST_BUFFER_SIZE 1024

void test_message_create_and_destroy(void) {
  struct cmsc_SipMessage *msg = NULL;
  cme_error_t err = cmsc_message_create(&msg, TEST_BUFFER_SIZE);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg);
  TEST_ASSERT_EQUAL(0, msg->present_mask);
  cmsc_message_destroy(&msg);
  TEST_ASSERT_NULL(msg);
}

void test_mark_and_check_field_present(void) {
  struct cmsc_SipMessage *msg = NULL;
  cme_error_t err = cmsc_message_create(&msg, TEST_BUFFER_SIZE);
  TEST_ASSERT_NULL(err);

  for (int field = 1; field <= cmsc_SipField_VIA_L; field <<= 1) {
    TEST_ASSERT_FALSE(
        cmsc_message_is_field_present(msg, (enum cmsc_SipField)field));
  }

  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_METHOD);
  TEST_ASSERT_TRUE(
      cmsc_message_is_field_present(msg, cmsc_SipField_SIP_METHOD));

  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_MSG_TYPE);
  cmsc_message_mark_field_present(msg, cmsc_SipField_VIA_L);

  TEST_ASSERT_TRUE(
      cmsc_message_is_field_present(msg, cmsc_SipField_SIP_METHOD));
  TEST_ASSERT_TRUE(
      cmsc_message_is_field_present(msg, cmsc_SipField_SIP_MSG_TYPE));
  TEST_ASSERT_TRUE(cmsc_message_is_field_present(msg, cmsc_SipField_VIA_L));

  cmsc_message_destroy(&msg);
}

void test_field_mask_bitwise_behavior(void) {
  struct cmsc_SipMessage *msg = NULL;
  cme_error_t err = cmsc_message_create(&msg, TEST_BUFFER_SIZE);
  TEST_ASSERT_NULL(err);

  cmsc_message_mark_field_present(msg, cmsc_SipField_IS_REQUEST);
  cmsc_message_mark_field_present(msg, cmsc_SipField_SIP_PROTO_VER);
  uint32_t expected_mask =
      cmsc_SipField_IS_REQUEST | cmsc_SipField_SIP_PROTO_VER;
  TEST_ASSERT_EQUAL(expected_mask, msg->present_mask);

  cmsc_message_destroy(&msg);
}

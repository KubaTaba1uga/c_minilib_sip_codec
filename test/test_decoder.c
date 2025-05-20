/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "unity_wrapper.h"
#include "utils.h"
#include "utils/bstring.h"
#include "utils/buffer.h"
#include "utils/decoder.h"
#include "utils/sipmsg.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {}
void tearDown(void) { cmsc_sipmsg_destroy_with_buf(&msg); }

void test_decode_to_header(void) {
  const char *raw_to_value = "To: <sip:bob@example.com>;tag=123abc";
  cme_error_t err;

  create_msg(raw_to_value, &msg);
  create_hdr(msg);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Ensure header was consumed
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Validate decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com",
                                 cmsc_bs_msg_to_string(&msg->to.uri, msg).buf,
                                 msg->to.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "123abc", cmsc_bs_msg_to_string(&msg->to.tag, msg).buf, msg->to.tag.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_TO));
}

void test_decode_from_header(void) {
  const char *raw_from_value = "From: <sip:alice@example.com>;tag=456def";
  cme_error_t err;

  create_msg(raw_from_value, &msg);
  create_hdr(msg);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Ensure header was consumed
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Validate decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com",
                                 cmsc_bs_msg_to_string(&msg->from.uri, msg).buf,
                                 msg->from.uri.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("456def",
                                 cmsc_bs_msg_to_string(&msg->from.tag, msg).buf,
                                 msg->from.tag.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_FROM));
}

void test_decode_cseq_header(void) {
  const char *raw_value = "CSeq: 42 INVITE";
  cme_error_t err;

  create_msg(raw_value, &msg);
  create_hdr(msg);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(42, msg->cseq.seq_number);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "INVITE", cmsc_bs_msg_to_string(&msg->cseq.method, msg).buf,
      msg->cseq.method.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_CSEQ));
}

void test_decode_call_id_header(void) {
  const char *raw_value = "Call-ID: abcd-1234";
  cme_error_t err;

  create_msg(raw_value, &msg);
  create_hdr(msg);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN("abcd-1234",
                                 cmsc_bs_msg_to_string(&msg->call_id, msg).buf,
                                 msg->call_id.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_CALL_ID));
}

void test_decode_max_forwards_header(void) {
  const char *raw_value = "Max-Forwards: 70";
  cme_error_t err;

  create_msg(raw_value, &msg);
  create_hdr(msg);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(70, msg->max_forwards);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_MAX_FORWARDS));
}

void test_decode_via_header_single(void) {
  const char *raw_value = "Via: SIP/2.0/UDP host.example.com;branch=z9hG4bK";
  cme_error_t err;

  create_msg(raw_value, &msg);
  create_hdr(msg);

  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&msg->vias);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "UDP", cmsc_bs_msg_to_string(&via->proto, msg).buf, via->proto.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN("host.example.com",
                                 cmsc_bs_msg_to_string(&via->sent_by, msg).buf,
                                 via->sent_by.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "z9hG4bK", cmsc_bs_msg_to_string(&via->branch, msg).buf, via->branch.len);
  TEST_ASSERT_TRUE(
      cmsc_sipmsg_is_field_present(msg, cmsc_SupportedSipHeaders_VIAS));
}

void test_decode_content_length_header(void) {
  const char *raw_value = "Content-Length: 123";
  cme_error_t err;

  create_msg(raw_value, &msg);
  create_hdr(msg);

  // Call decoder
  err = cmsc_decode_sip_headers(msg);
  TEST_ASSERT_NULL(err);

  // Custom check: if implemented in future decoder logic
  // For now just confirm that header was not erroneously left
  TEST_ASSERT_TRUE(STAILQ_EMPTY(&msg->sip_headers));

  // Optional: verify value if `content_length` field is added to
  // `cmsc_SipMessage`
  TEST_ASSERT_EQUAL(123, msg->content_length);
}

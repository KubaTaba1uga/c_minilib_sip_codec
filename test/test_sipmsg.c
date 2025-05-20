/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "unity_wrapper.h"
#include "utils.h"
#include "utils/bstring.h"
#include "utils/sipmsg.h"
#include <string.h>
#include <unity.h>

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) {
  cme_init();
  cme_error_t err = cmsc_sipmsg_create_with_buf(&msg);
  TEST_ASSERT_NULL(err);
}

void tearDown(void) { cmsc_sipmsg_destroy_with_buf(&msg); }

void test_create_with_buf_null(void) {
  cme_error_t err = cmsc_sipmsg_create_with_buf(NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_insert_valid_request_line(void) {
  const char *method = "INVITE";
  const char *uri = "sip:bob@example.com";
  const char *version = "SIP/2.0";

  cme_error_t err = cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "INVITE", cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg).buf,
      msg->request_line.sip_method.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:bob@example.com",
      cmsc_bs_msg_to_string(&msg->request_line.request_uri, msg).buf,
      msg->request_line.request_uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "SIP/2.0",
      cmsc_bs_msg_to_string(&msg->request_line.sip_proto_ver, msg).buf,
      msg->request_line.sip_proto_ver.len);
}

void test_insert_request_line_null_args(void) {
  cme_error_t err = cmsc_sipmsg_insert_request_line(
      6, "SIP/2.0", 17, "sip:bob@example.com", 6, NULL, msg);
  TEST_ASSERT_NOT_NULL(err);
}

void test_insert_valid_status_line(void) {
  const char *version = "SIP/2.0";
  const char *reason = "OK";
  uint32_t status_code = 200;

  cme_error_t err = cmsc_sipmsg_insert_status_line(
      strlen(version), version, strlen(reason), reason, status_code, msg);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL(status_code, msg->status_line.status_code);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "OK", cmsc_bs_msg_to_string(&msg->status_line.reason_phrase, msg).buf,
      msg->status_line.reason_phrase.len);
}

void test_insert_header_and_body(void) {
  const char *key = "X-Test";
  const char *val = "Hello";
  cme_error_t err =
      cmsc_sipmsg_insert_header(strlen(key), key, strlen(val), val, msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Test", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Hello", cmsc_bs_msg_to_string(&h->value, msg).buf, h->value.len);

  const char *body = "Hello Body";
  err = cmsc_sipmsg_insert_body(strlen(body), body, msg);
  TEST_ASSERT_NULL(err);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Hello Body", cmsc_bs_msg_to_string(&msg->body, msg).buf, msg->body.len);
  TEST_ASSERT_EQUAL(strlen(body), msg->content_length);
  TEST_ASSERT_TRUE(cmsc_sipmsg_is_field_present(
      msg, cmsc_SupportedSipHeaders_CONTENT_LENGTH));
}

void test_insert_header_key_only(void) {
  const char *key = "X-Only-Key";
  cme_error_t err = cmsc_sipmsg_insert_header(strlen(key), key, 0, NULL, msg);
  TEST_ASSERT_NULL(err);

  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Only-Key", cmsc_bs_msg_to_string(&h->key, msg).buf, h->key.len);
  TEST_ASSERT_EQUAL(0, h->value.len);
}

void test_insert_body_null_ptr(void) {
  cme_error_t err = cmsc_sipmsg_insert_body(5, "Hello", NULL);
  TEST_ASSERT_NOT_NULL(err);
}

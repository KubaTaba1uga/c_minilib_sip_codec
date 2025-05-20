/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"
#include <string.h>
#include <unity.h>

struct cmsc_SipMessage *msg;
const char *out_buf;

void setUp(void) {
  cme_init();
  msg = NULL;
  out_buf = NULL;
}

void tearDown(void) {
  free((void *)out_buf);
  cmsc_sipmsg_destroy_with_buf(&msg);
}

void test_generate_invite_exact_match(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  // Set request line: INVITE sip:alice@example.com SIP/2.0
  const char *method = "INVITE";
  const char *uri = "sip:alice@example.com";
  const char *version = "SIP/2.0";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Add one header: X-Test: value
  const char *key = "X-Test";
  const char *val = "value";
  TEST_ASSERT_NULL(
      cmsc_sipmsg_insert_header(strlen(key), key, strlen(val), val, msg));

  // Generate SIP message
  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  // Expected message string (note \r\n line endings)
  const char *expected = "INVITE sip:alice@example.com SIP/2.0\r\n"
                         "X-Test: value\r\n\r\n";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

void test_generate_full_sip_request_generic(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  const char *method = "INVITE";
  const char *uri = "sip:bob@example.com";
  const char *version = "SIP/2.0";

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("To"), "To",
                                             strlen("<sip:bob@example.com>"),
                                             "<sip:bob@example.com>", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(
      strlen("From"), "From", strlen("<sip:alice@example.com>;tag=123"),
      "<sip:alice@example.com>;tag=123", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("Call-ID"), "Call-ID",
                                             strlen("a84b4c76e66710"),
                                             "a84b4c76e66710", msg));

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(
      strlen("CSeq"), "CSeq", strlen("314159 INVITE"), "314159 INVITE", msg));

  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  const char *expected = "INVITE sip:bob@example.com SIP/2.0\r\n"
                         "To: <sip:bob@example.com>\r\n"
                         "From: <sip:alice@example.com>;tag=123\r\n"
                         "Call-ID: a84b4c76e66710\r\n"
                         "CSeq: 314159 INVITE\r\n\r\n";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}
void test_generate_very_long_sip_message(void) {
  // Create SIP message
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  // Insert request line
  const char *method = "OPTIONS";
  const char *uri = "sip:longmsg@example.com";
  const char *version = "SIP/2.0";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Embedded long header value (1023 'X's)
  const char *long_val = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                         "XXXXXXXXXXXXXXXXXXXXXXXXXX";

  // Insert long header
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_header(strlen("X-Long"), "X-Long",
                                             strlen(long_val), long_val, msg));

  // Generate SIP message
  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  // Allocate expected string with snprintf
  char expected_msg[2048];
  snprintf(expected_msg, 2048,
           "OPTIONS sip:longmsg@example.com SIP/2.0\r\n"
           "X-Long: %s\r\n"
           "\r\n",
           long_val);

  // Final assertion
  TEST_ASSERT_EQUAL_STRING(expected_msg, out_buf);
}
void test_generate_full_sip_request_invite(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  const char *method = "INVITE";
  const char *uri = "sip:bob@example.com";
  const char *version = "SIP/2.0";

  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Insert Via
  const char *proto = "SIP/2.0/UDP";
  const char *sent_by = "client.example.com";
  const char *branch = "z9hG4bK776asdhds";
  uint32_t ttl = 0;

  cme_error_t err =
      cmsc_sipmsg_insert_via(strlen(proto), proto, strlen(sent_by), sent_by, 0,
                             NULL, strlen(branch), branch, 0, NULL, ttl, msg);
  TEST_ASSERT_NULL(err);

  err = cmsc_sipmsg_insert_to(strlen("<sip:bob@example.com>"),
                              "<sip:bob@example.com>", strlen("abs456"),
                              "abs456", msg);
  TEST_ASSERT_NULL(err);

  err = cmsc_sipmsg_insert_from(strlen("<sip:alice@example.com>"),
                                "<sip:alice@example.com>", strlen("123"), "123",
                                msg);
  TEST_ASSERT_NULL(err);

  err = cmsc_sipmsg_insert_call_id(strlen("a84b4c76e66710"), "a84b4c76e66710",
                                   msg);
  TEST_ASSERT_NULL(err);

  err = cmsc_sipmsg_insert_cseq(strlen("INVITE"), "INVITE", 314159, msg);
  TEST_ASSERT_NULL(err);

  uint32_t out_len = 0;
  err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  const char *expected =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "Via: SIP/2.0/UDP client.example.com;branch=z9hG4bK776asdhds\r\n"
      "To: <sip:bob@example.com>;tag=abs456\r\n"
      "From: <sip:alice@example.com>;tag=123\r\n"
      "Call-ID: a84b4c76e66710\r\n"
      "CSeq: 314159 INVITE\r\n\r\n";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

void test_generate_sip_request_with_body(void) {
  TEST_ASSERT_NULL(cmsc_sipmsg_create_with_buf(&msg));

  // Insert request line
  const char *method = "POST";
  const char *uri = "sip:service@example.com";
  const char *version = "SIP/2.0";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_request_line(
      strlen(version), version, strlen(uri), uri, strlen(method), method, msg));

  // Insert Content-Type
  TEST_ASSERT_NULL(
      cmsc_sipmsg_insert_header(strlen("Content-Type"), "Content-Type",
                                strlen("text/plain"), "text/plain", msg));

  // Insert body
  const char *body = "Hello, this is the body";
  TEST_ASSERT_NULL(cmsc_sipmsg_insert_body(strlen(body), body, msg));

  // Generate SIP message
  uint32_t out_len = 0;
  cme_error_t err = cmsc_generate_sip(msg, &out_len, &out_buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(out_buf);

  // Expected output (hardcoded, like other tests)
  const char *expected = "POST sip:service@example.com SIP/2.0\r\n"
                         "Content-Length: 23\r\n"
                         "Content-Type: text/plain\r\n"
                         "\r\n"
                         "Hello, this is the body";

  TEST_ASSERT_EQUAL_STRING(expected, out_buf);
}

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "unity_wrapper.h"
#include <c_minilib_sip_codec.h>

#include "utils.h"
#include "utils/sipmsg.h"
#include "utils/bstring.h"

static struct cmsc_SipMessage *msg = NULL;

void setUp(void) { cme_init(); }
void tearDown(void) { cmsc_sipmsg_destroy(&msg); }

static void parse_msg(const char *raw) {
  cme_error_t err = cmsc_parse_sip((uint32_t)strlen(raw), raw, &msg);
  if (err) {
    puts(err->msg);
  }
  TEST_ASSERT_NULL(err);
}

void test_parse_basic_invite_request(void) {
  const char *raw =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bK776asdhds\r\n"
      "To: <sip:bob@example.com>\r\n"
      "From: <sip:alice@example.com>;tag=1928301774\r\n"
      "Call-ID: a84b4c76e66710\r\n"
      "CSeq: 314159 INVITE\r\n"
      "Max-Forwards: 13\r\n"
      "X-Custom:         Some custom value       \r\n"
      "\r\n";

  parse_msg(raw);

  // Request line checks
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "INVITE",
      cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg).buf,
      msg->request_line.sip_method.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:bob@example.com",
      cmsc_bs_msg_to_string(&msg->request_line.request_uri, msg).buf,
      msg->request_line.request_uri.len);

  // Decoded header fields
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:bob@example.com",
      cmsc_bs_msg_to_string(&msg->to.uri, msg).buf,
      msg->to.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:alice@example.com",
      cmsc_bs_msg_to_string(&msg->from.uri, msg).buf,
      msg->from.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "1928301774",
      cmsc_bs_msg_to_string(&msg->from.tag, msg).buf,
      msg->from.tag.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "INVITE",
      cmsc_bs_msg_to_string(&msg->cseq.method, msg).buf,
      msg->cseq.method.len);

  TEST_ASSERT_EQUAL(314159, msg->cseq.seq_number);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "a84b4c76e66710",
      cmsc_bs_msg_to_string(&msg->call_id, msg).buf,
      msg->call_id.len);

  TEST_ASSERT_EQUAL(13, msg->max_forwards);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "z9hG4bK776asdhds",
      cmsc_bs_msg_to_string(&msg->vias.stqh_first->branch, msg).buf,
      msg->vias.stqh_first->branch.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "pc33.example.com",
      cmsc_bs_msg_to_string(&msg->vias.stqh_first->sent_by, msg).buf,
      msg->vias.stqh_first->sent_by.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "UDP",
      cmsc_bs_msg_to_string(&msg->vias.stqh_first->proto, msg).buf,
      msg->vias.stqh_first->proto.len);

  // Remaining headers (only non-decoded should remain)
  struct cmsc_SipHeader *h = STAILQ_FIRST(&msg->sip_headers);
  TEST_ASSERT_NOT_NULL(h);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "X-Custom",
      cmsc_bs_msg_to_string(&h->key, msg).buf,
      h->key.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "Some custom value",
      cmsc_bs_msg_to_string(&h->value, msg).buf,
      h->value.len);

  h = STAILQ_NEXT(h, _next);
  TEST_ASSERT_NULL(h); // Ensure only 1 header remained after decoding
}
void test_parse_multiple_via_headers(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "Via: SIP/2.0/UDP first.example.com;branch=z1\r\n"
                    "Via: SIP/2.0/TCP second.example.com;branch=z2,SIP/2.0/WS "
                    "third.example.com;branch=z3\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=taggy\r\n"
                    "Call-ID: abc123\r\n"
                    "CSeq: 1 INVITE\r\n"
                    "Max-Forwards: 70\r\n"
                    "\r\n";

  parse_msg(raw);

  // Check all decoded fields
  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:bob@example.com",
      cmsc_bs_msg_to_string(&msg->to.uri, msg).buf,
      msg->to.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("sip:alice@example.com",
      cmsc_bs_msg_to_string(&msg->from.uri, msg).buf,
      msg->from.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("taggy",
      cmsc_bs_msg_to_string(&msg->from.tag, msg).buf,
      msg->from.tag.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("INVITE",
      cmsc_bs_msg_to_string(&msg->cseq.method, msg).buf,
      msg->cseq.method.len);

  TEST_ASSERT_EQUAL(1, msg->cseq.seq_number);

  MYTEST_ASSERT_EQUAL_STRING_LEN("abc123",
      cmsc_bs_msg_to_string(&msg->call_id, msg).buf,
      msg->call_id.len);

  TEST_ASSERT_EQUAL(70, msg->max_forwards);

  // Check Via list (should contain 3 entries)
  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&msg->vias);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("UDP",
      cmsc_bs_msg_to_string(&via->proto, msg).buf,
      via->proto.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("first.example.com",
      cmsc_bs_msg_to_string(&via->sent_by, msg).buf,
      via->sent_by.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("z1",
      cmsc_bs_msg_to_string(&via->branch, msg).buf,
      via->branch.len);

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("TCP",
      cmsc_bs_msg_to_string(&via->proto, msg).buf,
      via->proto.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("second.example.com",
      cmsc_bs_msg_to_string(&via->sent_by, msg).buf,
      via->sent_by.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("z2",
      cmsc_bs_msg_to_string(&via->branch, msg).buf,
      via->branch.len);

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NOT_NULL(via);
  MYTEST_ASSERT_EQUAL_STRING_LEN("WS",
      cmsc_bs_msg_to_string(&via->proto, msg).buf,
      via->proto.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("third.example.com",
      cmsc_bs_msg_to_string(&via->sent_by, msg).buf,
      via->sent_by.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN("z3",
      cmsc_bs_msg_to_string(&via->branch, msg).buf,
      via->branch.len);

  via = STAILQ_NEXT(via, _next);
  TEST_ASSERT_NULL(via); // Only 3 vias expected
}

void test_parse_message_with_body(void) {
  const char *raw = "INVITE sip:bob@example.com SIP/2.0\r\n"
                    "To: <sip:bob@example.com>\r\n"
                    "From: <sip:alice@example.com>;tag=tag123\r\n"
                    "Call-ID: call123\r\n"
                    "CSeq: 1 INVITE\r\n"
                    "Content-Length: 16\r\n"
                    "\r\n"
                    "Hello from body!";

  parse_msg(raw);

  // Basic field checks
  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:bob@example.com",
      cmsc_bs_msg_to_string(&msg->to.uri, msg).buf,
      msg->to.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "sip:alice@example.com",
      cmsc_bs_msg_to_string(&msg->from.uri, msg).buf,
      msg->from.uri.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "tag123",
      cmsc_bs_msg_to_string(&msg->from.tag, msg).buf,
      msg->from.tag.len);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "INVITE",
      cmsc_bs_msg_to_string(&msg->cseq.method, msg).buf,
      msg->cseq.method.len);

  TEST_ASSERT_EQUAL(1, msg->cseq.seq_number);

  MYTEST_ASSERT_EQUAL_STRING_LEN(
      "call123",
      cmsc_bs_msg_to_string(&msg->call_id, msg).buf,
      msg->call_id.len);

  // Check body
  struct cmsc_String body = cmsc_bs_msg_to_string(&msg->body, msg);
  TEST_ASSERT_EQUAL(16, body.len);
  TEST_ASSERT_EQUAL(16, msg->content_length);
  TEST_ASSERT_NOT_NULL(body.buf);
  TEST_ASSERT_NOT_EQUAL(0, body.len);  
  MYTEST_ASSERT_EQUAL_STRING_LEN("Hello from body!", body.buf, body.len);
}

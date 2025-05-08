// test/test_sip_proto_parse.d/test_sip_proto_parse.c
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "sip_msg/sip_msg.h"
#include "sip_proto/sip_modules/sip_field/parse/parse.h"
#include "sip_proto/sip_proto.h"
struct cmsc_SipMessage *msg = NULL;

#define TEST_BUFFER_SIZE 1024

void setUp(void) {
  cme_init();
  cmsc_sip_proto_init();

  cme_error_t err = cmsc_message_create(&msg, TEST_BUFFER_SIZE);
  TEST_ASSERT_NULL(err);
}

void tearDown(void) {
  free(msg);
  msg = NULL;
  cmsc_sip_proto_destroy();
  cme_destroy();
}

void test_parse_to_with_tag(void) {
  const char *line = "<sip:bob@domain.com>;tag=abc123";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->to.tag);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("tag=abc123", msg->to.tag);
  TEST_ASSERT_EQUAL_STRING("<sip:bob@domain.com>", msg->to.uri);
}

void test_parse_to_without_tag(void) {
  const char *line = "<sip:bob@domain.com>";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(msg->to.tag);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("<sip:bob@domain.com>", msg->to.uri);
}

void test_parse_to_with_display_name(void) {
  const char *line = "\"A. G. Bell\" <sip:alice@domain.com> ;tag=def456";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->to.display_name);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_NOT_NULL(msg->to.tag);

  TEST_ASSERT_EQUAL_STRING("\"A. G. Bell\"", msg->to.display_name);
  TEST_ASSERT_EQUAL_STRING("<sip:alice@domain.com>", msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("tag=def456", msg->to.tag);
}

void test_parse_to_no_display_name(void) {
  const char *line = "sip:+12125551212@server.phone2net.com;tag=887s";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_to(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_NOT_NULL(msg->to.tag);
  TEST_ASSERT_NULL(msg->to.display_name);

  TEST_ASSERT_EQUAL_STRING("sip:+12125551212@server.phone2net.com",
                           msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("tag=887s", msg->to.tag);
}

void test_parse_from_with_tag(void) {
  const char *line = "<sip:carol@domain.com>;tag=xyz789";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_from(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->from.tag);
  TEST_ASSERT_NOT_NULL(msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("tag=xyz789", msg->from.tag);
  TEST_ASSERT_EQUAL_STRING("<sip:carol@domain.com>", msg->from.uri);
}

void test_parse_from_without_tag(void) {
  const char *line = "<sip:carol@domain.com>";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_from(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NULL(msg->from.tag);
  TEST_ASSERT_NOT_NULL(msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("<sip:carol@domain.com>", msg->from.uri);
}

void test_parse_cseq_request_valid(void) {
  const char *line = "4711 INVITE";
  uint32_t len = strlen(line);

  msg->is_request = true;
  msg->sip_method = cmsc_SipMethod_INVITE;
  cme_error_t err = cmsc_parse_field_func_cseq(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL_STRING("4711 INVITE", msg->cseq);
}

void test_parse_cseq_request_invalid(void) {
  const char *line = "4711 INVITE";
  uint32_t len = strlen(line);

  msg->is_request = true;
  msg->sip_method = cmsc_SipMethod_NONE;
  cme_error_t err = cmsc_parse_field_func_cseq(len, line, msg);

  TEST_ASSERT_NOT_NULL(err);
}

void test_parse_cseq_response_valid(void) {
  const char *line = "4711 INVITE";
  uint32_t len = strlen(line);

  msg->is_request = false;
  msg->sip_method = cmsc_SipMethod_NONE;
  cme_error_t err = cmsc_parse_field_func_cseq(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL_STRING("4711 INVITE", msg->cseq);
}

void test_parse_via_single_entry(void) {
  const char *line = "SIP/2.0/UDP proxy.example.com;branch=z9hG4bK1";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_via(len, line, msg);

  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_STRING("proxy.example.com", msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP, msg->via_l.transp_proto);
  TEST_ASSERT_EQUAL_STRING("z9hG4bK1", msg->via_l.branch);
}

void test_parse_via_with_addr_param(void) {
  const char *line = "SIP/2.0/UDP host.domain.com;addr=10.0.0.1";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_via(len, line, msg);

  TEST_ASSERT_NULL(err);

  TEST_ASSERT_NOT_NULL(msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_STRING("host.domain.com", msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP, msg->via_l.transp_proto);
  TEST_ASSERT_NOT_NULL(msg->via_l.addr);
  TEST_ASSERT_EQUAL_STRING("10.0.0.1", msg->via_l.addr);
}

void test_parse_via_with_received_param(void) {
  const char *line = "SIP/2.0/UDP relay.domain.com;received=203.0.113.5";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_via(len, line, msg);

  TEST_ASSERT_NULL(err);

  TEST_ASSERT_NOT_NULL(msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_STRING("relay.domain.com", msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP, msg->via_l.transp_proto);
  TEST_ASSERT_NOT_NULL(msg->via_l.received);
  TEST_ASSERT_EQUAL_STRING("203.0.113.5", msg->via_l.received);
}

void test_parse_via_with_ttl_param(void) {
  const char *line = "SIP/2.0/UDP ttlproxy.net;branch=z9hG4bKdef;ttl=42";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_via(len, line, msg);

  TEST_ASSERT_NULL(err);

  TEST_ASSERT_NOT_NULL(msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_STRING("ttlproxy.net", msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP, msg->via_l.transp_proto);
  TEST_ASSERT_EQUAL_UINT32(42, msg->via_l.ttl);
}

void test_parse_via_multiple_values_one_header(void) {
  const char *line = "SIP/2.0/UDP a.example.com;branch=z9hG4bKa, SIP/2.0/UDP "
                     "b.example.com;branch=z9hG4bKb";
  uint32_t len = strlen(line);

  cme_error_t err = cmsc_parse_field_func_via(len, line, msg);

  TEST_ASSERT_NULL(err);

  // First entry
  TEST_ASSERT_NOT_NULL(msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_STRING("a.example.com", msg->via_l.sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP, msg->via_l.transp_proto);
  TEST_ASSERT_EQUAL_STRING("z9hG4bKa", msg->via_l.branch);

  // Second entry
  TEST_ASSERT_NOT_NULL(msg->via_l.next);
  TEST_ASSERT_EQUAL_STRING("b.example.com", msg->via_l.next->sent_by);
  TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP,
                        msg->via_l.next->transp_proto);
  TEST_ASSERT_EQUAL_STRING("z9hG4bKb", msg->via_l.next->branch);
}

/* void test_parse_via_multiple_header_calls(void) { */
/*   const char *via1 = "SIP/2.0/UDP node1.example.org;branch=z9hG4bK1"; */
/*   const char *via2 = "SIP/2.0/TCP node2.example.org;branch=z9hG4bK2"; */

/*   cme_error_t err1 = cmsc_parse_field_func_via(strlen(via1), via1, msg); */
/*   cme_error_t err2 = cmsc_parse_field_func_via(strlen(via2), via2, msg); */

/*   TEST_ASSERT_NULL(err1); */
/*   TEST_ASSERT_NULL(err2); */

/*   // First parsed header */
/*   TEST_ASSERT_NOT_NULL(msg->via_l.sent_by); */
/*   TEST_ASSERT_EQUAL_STRING("node1.example.org", msg->via_l.sent_by); */
/*   TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_UDP,
 * msg->via_l.transp_proto); */
/*   TEST_ASSERT_EQUAL_STRING("z9hG4bK1", msg->via_l.branch); */

/*   // Second parsed header */
/*   TEST_ASSERT_NOT_NULL(msg->via_l.next); */
/*   TEST_ASSERT_EQUAL_STRING("node2.example.org", msg->via_l.next->sent_by); */
/*   TEST_ASSERT_EQUAL_INT(cmsc_SipTransportProtocol_TCP,
 * msg->via_l.next->transp_proto); */
/*   TEST_ASSERT_EQUAL_STRING("z9hG4bK2", msg->via_l.next->branch); */
/* } */

void test_parse_minimal_invite(void) {
  const char *raw_msg =
      "INVITE sip:bob@example.com SIP/2.0\r\n"
      "To: alice@example.com\r\n"
      "From: bob@example.com\r\n"
      "CSeq: 4711 INVITE\r\n"
      "Call-ID: f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com\r\n"
      "Max-Forwards: 70\r\n"
      "\r\n";

  cme_error_t err = cmsc_sip_proto_parse(strlen(raw_msg), raw_msg, msg);
  TEST_ASSERT_NULL(err);

  // Basic structural checks
  TEST_ASSERT_TRUE(msg->is_request);
  TEST_ASSERT_EQUAL(cmsc_SipMsgType_INVITE, msg->sip_msg_type);
  TEST_ASSERT_EQUAL(2, msg->sip_proto_ver.major);
  TEST_ASSERT_EQUAL(0, msg->sip_proto_ver.minor);
  TEST_ASSERT_EQUAL(cmsc_SipMethod_INVITE, msg->sip_method);

  // To field
  TEST_ASSERT_NOT_NULL(msg->to.uri);
  TEST_ASSERT_EQUAL_STRING("alice@example.com", msg->to.uri);

  // From field
  TEST_ASSERT_NOT_NULL(msg->from.uri);
  TEST_ASSERT_EQUAL_STRING("bob@example.com", msg->from.uri);

  // CSeq field
  TEST_ASSERT_NOT_NULL(msg->cseq);
  TEST_ASSERT_EQUAL_STRING("4711 INVITE", msg->cseq);

  // Call-ID field
  TEST_ASSERT_NOT_NULL(msg->call_id);
  TEST_ASSERT_EQUAL_STRING("f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com",
                           msg->call_id);

  // Max-Forwards field
  TEST_ASSERT_EQUAL_INT(70, msg->max_forwards);

  // Presence mask
  uint32_t expected_mask = cmsc_SipField_IS_REQUEST | cmsc_SipField_SIP_METHOD |
                           cmsc_SipField_SIP_MSG_TYPE |
                           cmsc_SipField_SIP_PROTO_VER | cmsc_SipField_TO_URI |
                           cmsc_SipField_FROM_URI | cmsc_SipField_CSEQ |
                           cmsc_SipField_CALL_ID | cmsc_SipField_MAX_FORWARDS;

  TEST_ASSERT_EQUAL(expected_mask, msg->present_mask);
}

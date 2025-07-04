/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_H
#define C_MINILIB_SIP_CODEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/queue.h>

#include <c_minilib_error.h>

//
////
//////
/******************************************************************************
 ******************************************************************************/
//////////

/******************************************************************************
 *                             General                                        *
 ******************************************************************************/
struct cmsc_Buffer {
  uint32_t len;
  uint32_t size;
  const char *buf;
};

struct cmsc_BString {
  uint32_t len;
  uint32_t buf_offset;
};

struct cmsc_String {
  uint32_t len;
  const char *buf;
};

cme_error_t cmsc_init(void);
void cmsc_destroy(void);

/******************************************************************************
 *                             Message                                        *
 ******************************************************************************/
enum cmsc_SupportedSipHeaders {
  cmsc_SupportedSipHeaders_NONE = 0,
  cmsc_SupportedSipHeaders_REQUEST_LINE = 1,
  cmsc_SupportedSipHeaders_STATUS_LINE = 2,
  cmsc_SupportedSipHeaders_TO = 4,
  cmsc_SupportedSipHeaders_FROM = 8,
  cmsc_SupportedSipHeaders_CSEQ = 16,
  cmsc_SupportedSipHeaders_CALL_ID = 32,
  cmsc_SupportedSipHeaders_MAX_FORWARDS = 64,
  cmsc_SupportedSipHeaders_VIAS = 128,
  cmsc_SupportedSipHeaders_CONTENT_LENGTH = 256,
  // Add more fields here
  cmsc_SupportedSipHeaders_MAX,
};

struct cmsc_SipRequestLine {
  struct cmsc_BString sip_proto_ver;
  struct cmsc_BString request_uri;
  struct cmsc_BString sip_method;
};

struct cmsc_SipStatusLine {
  struct cmsc_BString sip_proto_ver;
  struct cmsc_BString reason_phrase;
  uint32_t status_code;
};

struct cmsc_SipHeader {
  struct cmsc_BString key;
  struct cmsc_BString value;
  STAILQ_ENTRY(cmsc_SipHeader) _next;
};

STAILQ_HEAD(cmsc_SipHeadersList, cmsc_SipHeader);

struct cmsc_SipHeaderTo {
  struct cmsc_BString uri;
  struct cmsc_BString tag;
};

struct cmsc_SipHeaderFrom {
  struct cmsc_BString uri;
  struct cmsc_BString tag;
};
struct cmsc_SipHeaderCSeq {
  struct cmsc_BString method;
  uint32_t seq_number;
};

struct cmsc_SipHeaderVia {
  struct cmsc_BString proto;
  struct cmsc_BString sent_by;
  struct cmsc_BString addr;
  struct cmsc_BString branch;
  struct cmsc_BString received;
  uint32_t ttl;
  STAILQ_ENTRY(cmsc_SipHeaderVia) _next;
};

STAILQ_HEAD(cmsc_SipViasList, cmsc_SipHeaderVia);

struct cmsc_SipMessage {
  uint32_t presence_mask;
  struct cmsc_SipRequestLine request_line;
  struct cmsc_SipStatusLine status_line;
  // Supported headers start
  struct cmsc_SipHeaderTo to;
  struct cmsc_SipHeaderFrom from;
  struct cmsc_SipHeaderCSeq cseq;
  struct cmsc_BString call_id;
  uint32_t max_forwards;
  struct cmsc_SipViasList vias;
  uint32_t content_length;
  // Supported headers end
  struct cmsc_SipHeadersList sip_headers;
  struct cmsc_BString body;
  struct cmsc_Buffer _buf;
};

/******************************************************************************
 *                             Parse                                          *
 ******************************************************************************/
cme_error_t cmsc_parse_sip(uint32_t buf_len, const char *buf,
                           struct cmsc_SipMessage **msg);
void cmsc_sipmsg_destroy(struct cmsc_SipMessage **msg);

/******************************************************************************
 *                             Generate                                       *
 ******************************************************************************/
cme_error_t cmsc_sipmsg_create_with_buf(struct cmsc_SipMessage **msg);
void cmsc_sipmsg_destroy_with_buf(struct cmsc_SipMessage **msg);

/* Generation of sipmsg allocates new buffer especially for dumped string. */

cme_error_t
cmsc_sipmsg_insert_request_line(uint32_t sip_ver_len, const char *sip_ver,
                                uint32_t req_uri_len, const char *req_uri,
                                uint32_t sip_method_len, const char *sip_method,
                                struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_status_line(uint32_t sip_ver_len,
                                           const char *sip_ver,
                                           uint32_t reason_phrase_len,
                                           const char *reason_phrase,
                                           uint32_t status_code,
                                           struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_header(uint32_t key_len, const char *key,
                                      uint32_t value_len, const char *value,
                                      struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_to(uint32_t uri_len, const char *uri,
                                  uint32_t tag_len, const char *tag,
                                  struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_from(uint32_t uri_len, const char *uri,
                                    uint32_t tag_len, const char *tag,
                                    struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_call_id(uint32_t call_id_len,
                                       const char *call_id,
                                       struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_cseq(uint32_t sip_method_len,
                                    const char *sip_method, uint32_t seq_number,
                                    struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_via(uint32_t proto_len, const char *proto,
                                   uint32_t sent_by_len, const char *sent_by,
                                   uint32_t addr_len, const char *addr,
                                   uint32_t branch_len, const char *branch,
                                   uint32_t received_len, const char *received,
                                   uint32_t ttl, struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_content_length(uint32_t content_length,
                                              struct cmsc_SipMessage *msg);

cme_error_t cmsc_sipmsg_insert_body(const uint32_t body_len, const char *body,
                                    struct cmsc_SipMessage *msg);

cme_error_t cmsc_generate_sip(const struct cmsc_SipMessage *msg,
                              uint32_t *buf_len, const char **buf);

static inline struct cmsc_String
cmsc_bs_msg_to_string(const struct cmsc_BString *src,
                      struct cmsc_SipMessage *msg) {
  return (struct cmsc_String){.buf = msg->_buf.buf + src->buf_offset,
                              .len = src->len};
}

static inline bool
cmsc_sipmsg_is_field_present(struct cmsc_SipMessage *msg,
                             enum cmsc_SupportedSipHeaders header_id) {
  return msg->presence_mask & header_id;
}

#endif

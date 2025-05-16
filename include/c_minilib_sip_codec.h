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
  // Add more fields here
  cmsc_SupportedSipHeaders_MAX,
};

struct cmsc_SipRequestLine {
  struct cmsc_String sip_proto_ver;
  struct cmsc_String request_uri;
  struct cmsc_String sip_method;
};

struct cmsc_SipStatusLine {
  struct cmsc_String sip_proto_ver;
  struct cmsc_String reason_phrase;
  uint32_t status_code;
};

struct cmsc_SipHeader {
  struct cmsc_String key;
  struct cmsc_String value;
  STAILQ_ENTRY(cmsc_SipHeader) _next;
};

STAILQ_HEAD(cmsc_SipHeadersList, cmsc_SipHeader);

struct cmsc_SipHeaderTo {
  struct cmsc_String uri;
  struct cmsc_String tag;
};

struct cmsc_SipHeaderFrom {
  struct cmsc_String uri;
  struct cmsc_String tag;
};
struct cmsc_SipHeaderCSeq {
  struct cmsc_String method;
  uint32_t seq_number;
};

struct cmsc_SipHeaderVia {
  struct cmsc_String proto;
  struct cmsc_String sent_by;
  struct cmsc_String addr;
  struct cmsc_String branch;
  struct cmsc_String received;
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
  struct cmsc_SipHeaderTo from;
  struct cmsc_SipHeaderCSeq cseq;
  struct cmsc_String call_id;
  uint32_t max_forwards;
  struct cmsc_SipViasList vias;
  // Supported headers end
  struct cmsc_SipHeadersList sip_headers;
  struct cmsc_Buffer body;
  struct cmsc_Buffer _buf; // This is buffer passed during parse
};

/******************************************************************************
 *                             Parse                                          *
 ******************************************************************************/
cme_error_t cmsc_parse_sip(uint32_t buf_len, const char *buf,
                           struct cmsc_SipMessage **msg);

/******************************************************************************
 *                             Generate                                       *
 ******************************************************************************/

#endif

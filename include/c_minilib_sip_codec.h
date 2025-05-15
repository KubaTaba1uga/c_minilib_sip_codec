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
  /* cmsc_SupportedSipHeaders_CSEQ = 64, */
  /* cmsc_SupportedSipHeaders_CALL_ID = 128, */
  /* cmsc_SupportedSipHeaders_MAX_FORWARDS = 256, */
  /* cmsc_SupportedSipHeaders_VIA_L = 512, */
  /* cmsc_SupportedSipHeaders_ALLOW = 1024, */
  /* cmsc_SupportedSipHeaders_ACCEPT = 2048, */
  /* cmsc_SupportedSipHeaders_EXPIRES = 4096, */
  /* cmsc_SupportedSipHeaders_SUBJECT = 8192, */
  /* cmsc_SupportedSipHeaders_ORGANIZATION = 16284, */
  /* cmsc_SupportedSipHeaders_USER_AGENT = 32562, */
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

struct cmsc_SipMessage {
  uint32_t presence_mask;
  struct cmsc_SipRequestLine request_line;
  struct cmsc_SipStatusLine status_line;
  // Supported headers start
  struct cmsc_SipHeaderTo to;
  struct cmsc_SipHeaderTo from;
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

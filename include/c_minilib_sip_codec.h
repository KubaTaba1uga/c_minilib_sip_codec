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
struct cmsc_SipHeader {
  struct cmsc_String key;
  struct cmsc_String value;
  STAILQ_ENTRY(cmsc_SipHeader) _next;
};

STAILQ_HEAD(cmsc_SipHeadersList, cmsc_SipHeader);

struct cmsc_SipMessage {
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

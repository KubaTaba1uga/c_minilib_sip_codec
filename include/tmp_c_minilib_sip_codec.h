/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_H
#define C_MINILIB_SIP_CODEC_H

#include "utils/fma.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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

typedef struct cmsc_SipMessage *cmsc_sipmsg_t;

/**
 * Initialize the sip codec library. Must be called before any parsing.
 */
cme_error_t cmsc_init(void);
/**
 * Clean up global parser state. Call after all parsing is complete.
 */
void cmsc_destroy(void);

/******************************************************************************
 *                             Scheme                                         *
 ******************************************************************************/
struct cmsc_SchemeField {
  const char *id;

  bool is_list; // Fields wich can occur multiple times are
                //   represented as lists.

  bool (*is_field_func)(         // This function is optional if none
      const uint32_t buffer_len, // provided default one is used.
      const char *buffer);

  cme_error_t (*parse_field_func)( // This function is mandatory there
      const uint32_t buffer_len,   // is no default one.
      const char *buffer, cmsc_sipmsg_t msg);

  void *generate_field_func;
};

enum cmsc_SipMethod {
  cmsc_SipMethod_NONE = 0,
  cmsc_SipMethod_INVITE,
  // Add more sip methods here
  cmsc_SipMethod_MAX,
};

enum cmsc_SipMsgType {
  cmsc_SipMsgType_NONE = 0,
  cmsc_SipMsgType_INVITE,
  cmsc_SipMsgType_200_OK,
  // Add more message types here
  cmsc_SipMsgType_MAX,
};

static inline char *
cmsc_dump_sip_method_to_str(enum cmsc_SipMethod msg_method) {
  switch (msg_method) {
  case cmsc_SipMethod_INVITE:
    return "INVITE";
  // Add more sip methods here
  default:
    return "UNKOWN";
  }
}

static inline enum cmsc_SipMethod
cmsc_map_sip_msg_type_to_sip_method(enum cmsc_SipMsgType msg_type) {
  switch (msg_type) {
  case cmsc_SipMsgType_INVITE:
    return cmsc_SipMethod_INVITE;
  // Add more sip methods here
  default:
    return cmsc_SipMethod_NONE;
  }
}

struct cmsc_SchemeFields {
  struct cmsc_SchemeField *fields;
  uint32_t size;
  uint32_t len;
};

struct cmsc_Scheme {
  const char *sip_msg_type_str;
  enum cmsc_SipMsgType sip_msg_type;
  struct cmsc_SchemeFields mandatory;
  struct cmsc_SchemeFields optional;
};

/******************************************************************************
 *                             Message                                        *
 ******************************************************************************/
struct cmsc_SipProtoVer {
  uint32_t major;
  uint32_t minor;
};

enum cmsc_SipTransportProtocol {
  cmsc_SipTransportProtocol_NONE = 0,
  cmsc_SipTransportProtocol_UDP,
  // Add more transport protocols here
  cmsc_SipTransportProtocol_MAX,
};

struct cmsc_SipVia {
  enum cmsc_SipTransportProtocol transp_proto;
  const char *sent_by;
  const char *addr;
  const char *branch;
  const char *received;
  uint32_t ttl;
  struct cmsc_SipVia *next; // This is linked list
};

struct cmsc_SipTo {
  const char *uri;
  const char *tag;
  const char *display_name;
};

struct cmsc_SipFrom {
  const char *uri;
  const char *tag;
  const char *display_name;
};

struct cmsc_SipMessage {
  uint32_t present_mask; // If a field is present corresponding bit is set to 1,
                         // You can check for field presence like this:
                         //     m->present & cmsc_SipField_FIELD_NAME.
  bool is_request;
  uint32_t sip_status_code; // This is set to >0 only for responses.
  enum cmsc_SipMethod sip_method;
  enum cmsc_SipMsgType sip_msg_type;
  struct cmsc_SipProtoVer sip_proto_ver;
  struct cmsc_SipVia via_l;
  struct cmsc_SipTo to;
  struct cmsc_SipFrom from;
  const char *cseq;
  const char *call_id;
  uint32_t max_forwards;
  // Add more fields here
  struct cmsc_FamBuffer _buffer; // This field needs to be last
};

/******************************************************************************
 *                             Parse                                          *
 ******************************************************************************/
cme_error_t cmsc_parse_sip_msg(const uint32_t n, const char *buffer,
                               cmsc_sipmsg_t *msg);

/******************************************************************************
 *                             Generate                                       *
 ******************************************************************************/
cme_error_t cmsc_generate_sip_msg(const cmsc_sipmsg_t msg, uint32_t *n,
                                  char **buffer);

#endif

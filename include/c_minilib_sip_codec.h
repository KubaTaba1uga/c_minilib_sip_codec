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
  void *is_field_func; // This function is optional if none provided default one
                       // is used.
  void *parse_field_func;
  void *generate_field_func;
};

enum cmsc_SipMethod {
  cmsc_SipMethod_INVITE,
};

enum cmsc_SipMsgType {
  cmsc_SipMsgType_INVITE,
  cmsc_SipMsgType_200_OK,
  // Add more message types here
};

struct cmsc_Scheme {
  uint32_t mandatory_fields_len;
  struct cmsc_SchemeField *mandatory_fields;
  uint32_t optional_fields_len;
  struct cmsc_SchemeField *optional_fields;
  enum cmsc_SipMsgType sip_msg_type;
};

/******************************************************************************
 *                             Message                                        *
 ******************************************************************************/
#define CMSC_STRINGSBUFFER_MAX 2048
struct cmsc_StringsBuffer {
  uint32_t buffer_len;
  char buffer[CMSC_STRINGSBUFFER_MAX];
};

struct cmsc_SipProtoVer {
  uint32_t major;
  uint32_t minor;
};

enum cmsc_SipTransportProtocol {
  cmsc_SipTransportProtocol_UDP,
};

struct cmsc_SipVia {
  struct cmsc_SipProtoVer sip_proto_ver;
  enum cmsc_SipTransportProtocol transp_proto;
  const char *addr;
  const char *branch;
  const char *received;
  struct cmsc_SipVia *next; // This is linked list
};

struct cmsc_SipMessage {
  bool is_request;
  uint32_t sip_status_code; // This is set to >0 only for responses.
  enum cmsc_SipMethod sip_method;
  enum cmsc_SipMsgType sip_msg_type;
  struct cmsc_SipProtoVer sip_proto_ver;
  struct cmsc_SipVia via_l;
  // Add more fields here
};

typedef struct cmsc_SipMessage *cmsc_sipmsg_t;
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

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_H
#define C_MINILIB_SIP_CODEC_H

#include <stdint.h>
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
enum cmsc_LogLevels {
  cmsc_LogLevels_DEBUG = 0,
  cmsc_LogLevels_INFO,
  cmsc_LogLevels_WARNING,
  cmsc_LogLevels_ERROR,
};

struct cmsc_Settings {
  void (*log_func)(enum cmsc_LogLevels log_lvl, const char *msg);
};

void cmsc_configure(struct cmsc_Settings settings);
cme_error_t cmsc_init(void);
void cmsc_destroy(void);

struct cmsc_CharBufferView {
  const uint8_t *buf;
  const uint32_t buf_len;
};

/******************************************************************************
 *                             Sip Message *
 ******************************************************************************/

/* Fields are pretty loose concept. SIP header is a field, but
   also body is a field or request line is parsed into multiple fields.
   You can get fields from cmsc_sipmsg_t. Fields interpret different
   values in ctx of single sip message.
*/

enum cmsc_SupportedMessages {
  cmsc_SupportedMessages_NONE = 0,
  cmsc_SupportedMessages_INVITE = 1,
  cmsc_SupportedMessages_ACK = 2,
  cmsc_SupportedMessages_OPTIONS = 4,
  cmsc_SupportedMessages_CANCEL = 8,
  cmsc_SupportedMessages_BYE = 16,
  cmsc_SupportedMessages_200_OK = 32,
  // Add more messages here
  cmsc_SupportedMessages_MAX,
};

enum cmsc_SupportedFields {
  cmsc_SupportedFields_NONE = 0,
  cmsc_SupportedFields_SUPPORTED_MSG = 1,
  cmsc_SupportedFields_REQUEST_LINE = 2,
  cmsc_SupportedFields_STATUS_LINE = 4,
  cmsc_SupportedFields_IS_REQUEST = 8,
  cmsc_SupportedFields_TO = 16,
  cmsc_SupportedFields_FROM = 32,
  cmsc_SupportedFields_CSEQ = 64,
  cmsc_SupportedFields_CALL_ID = 128,
  cmsc_SupportedFields_MAX_FORWARDS = 256,
  cmsc_SupportedFields_VIA_L = 512,
  cmsc_SupportedFields_ALLOW = 1024,
  // Add more fields here
  cmsc_SupportedFields_MAX,
};

struct cmsc_Field_SipProtoVer {
  uint32_t major;
  uint32_t minor;
};

struct cmsc_Field_RequestLine {
  struct cmsc_Field_SipProtoVer sip_proto_ver;
  const char *request_uri;
};

struct cmsc_Field_StatusLine {
  struct cmsc_Field_SipProtoVer sip_proto_ver;
  const char *reason_phrase;
  uint32_t status_code;
};

struct cmsc_Field_To {
  const char *uri;
  const char *tag;
  const char *display_name;
};

struct cmsc_Field_From {
  const char *uri;
  const char *tag;
  const char *display_name;
};

struct cmsc_Field_CSeq {
  const char *method;
  uint32_t seq_number;
};

enum cmsc_TransportProtocols {
  cmsc_TransportProtocols_NONE = 0,
  cmsc_TransportProtocols_UDP,
  // Add more transport protocols here
  cmsc_TransportProtocols_MAX,
};

static inline const char *
cmsc_sipmsg_dump_transp_proto(enum cmsc_TransportProtocols trans_proto) {
  switch (trans_proto) {
  case cmsc_TransportProtocols_UDP:
    return "UDP";
  default:
    return "UNKOWN";
  }
}

struct cmsc_Field_Via {
  enum cmsc_TransportProtocols transp_proto;
  const char *sent_by;
  const char *addr;
  const char *branch;
  const char *received;
  uint32_t ttl;
  STAILQ_ENTRY(cmsc_Field_Via) vias_l; // This is linked list
};

struct cmsc_Field_Accept {
  const char *mime;                          // We are not parsing MIME args.
  STAILQ_ENTRY(cmsc_Field_Accept) accepts_l; // This is linked list
};

typedef struct cmsc_SipMsg *cmsc_sipmsg_t;

bool cmsc_sipmsg_get_is_request(const cmsc_sipmsg_t msg);
struct cmsc_Field_RequestLine *
cmsc_sipmsg_get_request_line(const cmsc_sipmsg_t msg);
struct cmsc_Field_StatusLine *
cmsc_sipmsg_get_response_line(const cmsc_sipmsg_t msg);
struct cmsc_Field_To *cmsc_sipmsg_get_to(const cmsc_sipmsg_t msg);
struct cmsc_Field_From *cmsc_sipmsg_get_from(const cmsc_sipmsg_t msg);
struct cmsc_CharBufferView *cmsc_sipmsg_get_body(const cmsc_sipmsg_t msg);
void cmsc_sipmsg_destroy(cmsc_sipmsg_t *msg);

/******************************************************************************
 *                             Parser                                         *
 ******************************************************************************/
enum cmsc_ParserStates {
  cmsc_ParserStates_MsgEmpty = 0,
  cmsc_ParserStates_ParsingHeaders,
  cmsc_ParserStates_ParsingBody,
  cmsc_ParserStates_MsgReady,
};

typedef struct cmsc_Parser *cmsc_parser_t;

cme_error_t cmsc_parser_create(cmsc_parser_t *parser);
void cmsc_parser_destroy(cmsc_parser_t *parser);
enum cmsc_ParserStates cmsc_parser_get_state(cmsc_parser_t parser);
cme_error_t cmsc_parser_feed_data(struct cmsc_CharBufferView data,
                                  cmsc_parser_t *parser);
cmsc_sipmsg_t cmsc_parser_pop_msg(cmsc_parser_t parser);

#endif

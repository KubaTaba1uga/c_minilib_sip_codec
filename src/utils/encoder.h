/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_DECODER_H
#define C_MINILIB_SIP_CODEC_DECODER_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/bstring.h"
#include "utils/buffer.h"
#include "utils/sipmsg.h"
#include "utils/tag_iterator.h"
#include <stdint.h>

struct cmsc_EncoderLogic {
  cme_error_t (*encode_func)(const struct cmsc_SipMessage *,
                             struct cmsc_Buffer *);
  enum cmsc_SupportedSipHeaders id;
};

static inline cme_error_t cmsc_encode_hdr_to(const struct cmsc_SipMessage *msg,
                                             struct cmsc_Buffer *buf);
static inline cme_error_t
cmsc_encode_hdr_from(const struct cmsc_SipMessage *msg,
                     struct cmsc_Buffer *buf);
static inline cme_error_t
cmsc_encode_hdr_call_id(const struct cmsc_SipMessage *msg,
                        struct cmsc_Buffer *buf);
static inline cme_error_t
cmsc_encode_hdr_cseq(const struct cmsc_SipMessage *msg,
                     struct cmsc_Buffer *buf);

static inline cme_error_t
cmsc_encode_request_line(const struct cmsc_SipMessage *msg,
                         struct cmsc_Buffer *buf) {
  return cmsc_buffer_finsert(
      buf, NULL, "%.*s %.*s %.*s\r\n", msg->request_line.sip_method.len,
      cmsc_bs_msg_to_string(&msg->request_line.sip_method,
                            (struct cmsc_SipMessage *)msg)
          .buf,
      msg->request_line.request_uri.len,
      cmsc_bs_msg_to_string(&msg->request_line.request_uri,
                            (struct cmsc_SipMessage *)msg)
          .buf,
      msg->request_line.sip_proto_ver.len,
      cmsc_bs_msg_to_string(&msg->request_line.sip_proto_ver,
                            (struct cmsc_SipMessage *)msg)
          .buf);
};

static inline cme_error_t
cmsc_encode_status_line(const struct cmsc_SipMessage *msg,
                        struct cmsc_Buffer *buf) {
  return cmsc_buffer_finsert(
      buf, NULL, "%.*s %d %.*s\r\n", msg->status_line.sip_proto_ver.len,
      cmsc_bs_msg_to_string(&msg->status_line.sip_proto_ver,
                            (struct cmsc_SipMessage *)msg)
          .buf,
      msg->status_line.status_code, msg->status_line.reason_phrase.len,
      cmsc_bs_msg_to_string(&msg->status_line.reason_phrase,
                            (struct cmsc_SipMessage *)msg)
          .buf);
};

static inline cme_error_t
cmsc_encode_generic_hdr(const struct cmsc_SipMessage *msg,
                        const struct cmsc_SipHeader *hdr,
                        struct cmsc_Buffer *buf) {
  return cmsc_buffer_finsert(
      buf, NULL, "%.*s: %.*s\r\n", hdr->key.len,
      cmsc_bs_msg_to_string(&hdr->key, (struct cmsc_SipMessage *)msg).buf,
      hdr->value.len,
      cmsc_bs_msg_to_string(&hdr->value, (struct cmsc_SipMessage *)msg).buf);
};

static inline cme_error_t
cmsc_encode_sip_headers(const struct cmsc_SipMessage *msg,
                        struct cmsc_Buffer *buf) {
  static struct cmsc_EncoderLogic encoders[] = {
      {.encode_func = cmsc_encode_hdr_to, .id = cmsc_SupportedSipHeaders_TO},
      {.encode_func = cmsc_encode_hdr_from,
       .id = cmsc_SupportedSipHeaders_FROM},
      {.encode_func = cmsc_encode_hdr_call_id,
       .id = cmsc_SupportedSipHeaders_CALL_ID},
      {.encode_func = cmsc_encode_hdr_cseq,
       .id = cmsc_SupportedSipHeaders_CSEQ}};
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  for (uint32_t i = 0; i < sizeof(encoders) / sizeof(struct cmsc_EncoderLogic);
       i++) {
    if (!cmsc_sipmsg_is_field_present((struct cmsc_SipMessage *)msg,
                                      encoders[i].id)) {
      continue;
    }

    err = encoders[i].encode_func(msg, buf);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t cmsc_encode_hdr_to(const struct cmsc_SipMessage *msg,
                                             struct cmsc_Buffer *buf) {
  cme_error_t err;
  if (msg->to.tag.len > 0) {
    err = cmsc_buffer_finsert(
        buf, NULL, "%.*s: %.*s;tag=%.*s\r\n", strlen("To"), "To",
        msg->to.uri.len,
        cmsc_bs_msg_to_string(&msg->to.uri, (struct cmsc_SipMessage *)msg).buf,
        msg->to.tag.len,
        cmsc_bs_msg_to_string(&msg->to.tag, (struct cmsc_SipMessage *)msg).buf);
  } else {
    err = cmsc_buffer_finsert(
        buf, NULL, "%.*s: %.*s\r\n", strlen("To"), "To", msg->to.uri.len,
        cmsc_bs_msg_to_string(&msg->to.uri, (struct cmsc_SipMessage *)msg).buf);
  }

  return err;
};

static inline cme_error_t
cmsc_encode_hdr_from(const struct cmsc_SipMessage *msg,
                     struct cmsc_Buffer *buf) {
  cme_error_t err;
  if (msg->to.tag.len > 0) {
    err = cmsc_buffer_finsert(
        buf, NULL, "%.*s: %.*s;tag=%.*s\r\n", strlen("From"), "From",
        msg->from.uri.len,
        cmsc_bs_msg_to_string(&msg->from.uri, (struct cmsc_SipMessage *)msg)
            .buf,
        msg->from.tag.len,
        cmsc_bs_msg_to_string(&msg->from.tag, (struct cmsc_SipMessage *)msg)
            .buf);
  } else {
    err = cmsc_buffer_finsert(
        buf, NULL, "%.*s: %.*s\r\n", strlen("From"), "From", msg->from.uri.len,
        cmsc_bs_msg_to_string(&msg->from.uri, (struct cmsc_SipMessage *)msg)
            .buf);
  }

  return err;
};

static inline cme_error_t
cmsc_encode_hdr_call_id(const struct cmsc_SipMessage *msg,
                        struct cmsc_Buffer *buf) {
  return cmsc_buffer_finsert(
      buf, NULL, "%.*s: %.*s\r\n", strlen("Call-ID"), "Call-ID",
      msg->call_id.len,
      cmsc_bs_msg_to_string(&msg->call_id, (struct cmsc_SipMessage *)msg).buf);
};

static inline cme_error_t
cmsc_encode_hdr_cseq(const struct cmsc_SipMessage *msg,
                     struct cmsc_Buffer *buf) {
  return cmsc_buffer_finsert(
      buf, NULL, "%.*s: %d %.*s\r\n", strlen("CSeq"), "CSeq",
      msg->cseq.seq_number, msg->cseq.method.len,
      cmsc_bs_msg_to_string(&msg->cseq.method, (struct cmsc_SipMessage *)msg)
          .buf);
};

#endif

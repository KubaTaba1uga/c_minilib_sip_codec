/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_SIPMSG_H
#define C_MINILIB_SIP_CODEC_SIPMSG_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/dynamic_buffer.h"

#ifndef CMSC_SIPMSG_DYNBUF_SIZE
#define CMSC_SIPMSG_DYNBUF_SIZE 512
#endif

enum cmsc_SupportedMessages {
  cmsc_SupportedMessages_NONE = 0,
  cmsc_SupportedMessages_INVITE,
  cmsc_SupportedMessages_200_OK,
  cmsc_SupportedMessages_MAX,
};

struct cmsc_SipMsg {
  uint32_t presence_mask;
  bool is_request;
  enum cmsc_SupportedMessages supmsg;
  struct cmsc_Field_SipProtoVer sip_proto_ver;
  struct cmsc_Field_To to;
  struct cmsc_Field_From from;
  struct cmsc_Field_CSeq cseq;
  struct cmsc_DynamicBuffer content;
};

static inline cme_error_t cmsc_sipmsg_create(cmsc_sipmsg_t *sipmsg) {
  cmsc_sipmsg_t local_msg;
  cme_error_t err;

  if (!sipmsg) {
    err = cme_error(EINVAL, "`sipmsg` cannot be NULL");
    goto error_out;
  }

  local_msg = calloc(1, sizeof(struct cmsc_SipMsg) +
                            sizeof(char) * CMSC_SIPMSG_DYNBUF_SIZE);
  if (!local_msg) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_msg`");
    goto error_out;
  }

  if ((err = cmsc_dynbuf_init(CMSC_SIPMSG_DYNBUF_SIZE, &local_msg->content))) {
    goto error_out;
  }

  *sipmsg = local_msg;

  return 0;

error_out:
  return cme_return(err);
};

static inline const char *cmsc_sipmsg_insert_str(const uint32_t buffer_len,
                                                 const char *buffer,
                                                 cmsc_sipmsg_t *sipmsg) {
  if (!buffer || !sipmsg) {
    goto error_out;
  }

  return cmsc_dynbuf_put_word(buffer_len, (char *)buffer, (void **)sipmsg,
                              sizeof(struct cmsc_SipMsg), &(*sipmsg)->content);
error_out:
  return NULL;
};

static inline const char *
cmsc_dump_supported_messages_string(enum cmsc_SupportedMessages supmsg) {
  static const char *strings[] = {"INVITE", "200 OK"};

  if (supmsg <= cmsc_SupportedMessages_NONE ||
      supmsg >= cmsc_SupportedMessages_MAX) {
    return NULL;
  }

  return strings[supmsg - 1];
}

static inline void
cmsc_sipmsg_mark_field_present(struct cmsc_SipMsg *message,
                               enum cmsc_SupportedFields field) {
  message->presence_mask = message->presence_mask | field;
}

static inline bool
cmsc_sipmsg_is_field_present(struct cmsc_SipMsg *message,
                             enum cmsc_SupportedFields field) {
  return message->presence_mask & field;
}

#endif

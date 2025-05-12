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
#include "utils/list.h"

#ifndef CMSC_SIPMSG_DYNBUF_SIZE
#define CMSC_SIPMSG_DYNBUF_SIZE 512
#endif

#define CMSC_SUPPORTED_MESSAGES_FOREACH(var)                                   \
  for (uint32_t var = 1; var < cmsc_SupportedMessages_MAX; var *= 2)

#define CMSC_SUPPORTED_FIELDS_FOREACH(var)                                     \
  for (uint32_t var = 1; var < cmsc_SupportedFields_MAX; var *= 2)

struct cmsc_SipMsg {
  uint32_t presence_mask;
  bool is_request;
  enum cmsc_SupportedMessages supmsg;
  struct cmsc_Field_RequestLine request_line;
  struct cmsc_Field_StatusLine status_line;
  struct cmsc_Field_To to;
  struct cmsc_Field_From from;
  struct cmsc_Field_CSeq cseq;
  const char *call_id;
  uint32_t max_forwards;
  struct cmsc_ViaList via;
  uint32_t allow_mask;
  struct cmsc_AcceptList accept;
  uint32_t expires;
  const char *subject;
  const char *organization;
  const char *user_agent;
  // Add new fields here
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

  STAILQ_INIT(&local_msg->via);
  STAILQ_INIT(&local_msg->accept);

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
  static const char *strings[] = {
      [cmsc_SupportedMessages_INVITE] = "INVITE",
      [cmsc_SupportedMessages_ACK] = "ACK",
      [cmsc_SupportedMessages_OPTIONS] = "OPTIONS",
      [cmsc_SupportedMessages_CANCEL] = "CANCEL",
      [cmsc_SupportedMessages_BYE] = "BYE",
      [cmsc_SupportedMessages_200_OK] = "200 OK",

  };

  if (supmsg <= cmsc_SupportedMessages_NONE ||
      supmsg >= cmsc_SupportedMessages_MAX) {
    return NULL;
  }

  return strings[supmsg];
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

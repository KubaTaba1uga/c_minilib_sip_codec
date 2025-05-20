/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_DECODER_H
#define C_MINILIB_SIP_CODEC_DECODER_H

#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/bstring.h"
#include "utils/sipmsg.h"
#include "utils/tag_iterator.h"

struct cmsc_DecoderLogic {
  struct cmsc_String header_id;
  cme_error_t (*decode_func)(const struct cmsc_SipHeader *sip_header,
                             struct cmsc_SipMessage *msg);
};

static inline cme_error_t
cmsc_decode_func_to(const struct cmsc_SipHeader *sip_header,
                    struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_via(const struct cmsc_SipHeader *sip_header,
                     struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_from(const struct cmsc_SipHeader *sip_header,
                      struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_cseq(const struct cmsc_SipHeader *sip_header,
                      struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_call_id(const struct cmsc_SipHeader *sip_header,
                         struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_max_forwards(const struct cmsc_SipHeader *sip_header,
                              struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_content_length(const struct cmsc_SipHeader *sip_header,
                                struct cmsc_SipMessage *msg);

static inline cme_error_t cmsc_decode_sip_headers(struct cmsc_SipMessage *msg) {
  static struct cmsc_DecoderLogic decoders[] = {
      {.header_id = {.buf = "To", .len = sizeof("To") - 1},
       .decode_func = cmsc_decode_func_to},
      {.header_id = {.buf = "Via", .len = sizeof("Via") - 1},
       .decode_func = cmsc_decode_func_via},
      {.header_id = {.buf = "From", .len = sizeof("From") - 1},
       .decode_func = cmsc_decode_func_from},
      {.header_id = {.buf = "CSeq", .len = sizeof("CSeq") - 1},
       .decode_func = cmsc_decode_func_cseq},
      {.header_id = {.buf = "Call-ID", .len = sizeof("Call-ID") - 1},
       .decode_func = cmsc_decode_func_call_id},
      {.header_id = {.buf = "Max-Forwards", .len = sizeof("Max-Forwards") - 1},
       .decode_func = cmsc_decode_func_max_forwards},
      {.header_id = {.buf = "Content-Length",
                     .len = sizeof("Content-Length") - 1},
       .decode_func = cmsc_decode_func_content_length},
  };
  cme_error_t err;

  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  struct cmsc_SipHeader *generic_header;
  struct cmsc_SipHeader *next_header;
  generic_header = STAILQ_FIRST(&msg->sip_headers);

  while (generic_header != NULL) {
    next_header = STAILQ_NEXT(generic_header, _next);

    cmsc_bs_trimm(&generic_header->value, ' ', msg);

    // Parse generic header
    bool is_match = false;
    for (uint32_t i = 0;
         i < sizeof(decoders) / sizeof(struct cmsc_DecoderLogic); i++) {
      if (strncmp(decoders[i].header_id.buf,
                  cmsc_bs_msg_to_string(&generic_header->key, msg).buf,
                  generic_header->key.len) == 0) {
        is_match = true;
        err = decoders[i].decode_func(generic_header, msg);
        if (err) {
          goto error_out;
        }

        break;
      }
    }

    if (is_match) {
      STAILQ_REMOVE(&msg->sip_headers, generic_header, cmsc_SipHeader, _next);
      free(generic_header);
    }

    generic_header = next_header;
  }
  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsc_decode_func_to(const struct cmsc_SipHeader *sip_header,
                    struct cmsc_SipMessage *msg) {
  struct cmsc_ArgIterator iter;
  cme_error_t err;

  err = cmsc_arg_iterator_init(cmsc_bs_msg_to_string(&sip_header->value, msg),
                               &iter);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&iter))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      cmsc_s_trimm(&iter.value, '<');
      cmsc_s_trimm(&iter.value, '>');

      msg->to.uri = cmsc_s_msg_to_bstring(&iter.value, msg);
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_TO);
      break;
    }
    case cmsc_ArgNextResults_ARG: {
      if (strncmp("tag", iter.arg_key.buf, iter.arg_key.len) == 0) {
        msg->to.tag = cmsc_s_msg_to_bstring(&iter.arg_value, msg);
      }
      break;
    }
    default:;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsc_decode_func_from(const struct cmsc_SipHeader *sip_header,
                      struct cmsc_SipMessage *msg) {
  struct cmsc_ArgIterator iter;
  cme_error_t err;

  err = cmsc_arg_iterator_init(cmsc_bs_msg_to_string(&sip_header->value, msg),
                               &iter);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&iter))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      cmsc_s_trimm(&iter.value, '<');
      cmsc_s_trimm(&iter.value, '>');

      msg->from.uri = cmsc_s_msg_to_bstring(&iter.value, msg);
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_FROM);
      break;
    }
    case cmsc_ArgNextResults_ARG: {
      if (strncmp("tag", iter.arg_key.buf, iter.arg_key.len) == 0) {
        msg->from.tag = cmsc_s_msg_to_bstring(&iter.arg_value, msg);
      }
      break;
    }
    default:;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsc_decode_func_cseq(const struct cmsc_SipHeader *sip_header,
                      struct cmsc_SipMessage *msg) {
  const char *digit;
  const char *method = digit =
      cmsc_bs_msg_to_string(&sip_header->value, msg).buf;
  const char *end = method + sip_header->value.len;
  while (method != end) {
    if (!isdigit(*method)) {
      break;
    }
    method++;
  }

  while (isspace(*method)) {
    method++;
  }

  msg->cseq.seq_number = atoi(digit);
  msg->cseq.method = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.len = end - method, .buf = method}, msg);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CSEQ);
  return 0;
}

static inline cme_error_t
cmsc_decode_func_call_id(const struct cmsc_SipHeader *sip_header,
                         struct cmsc_SipMessage *msg) {
  msg->call_id = sip_header->value;
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CALL_ID);
  return 0;
};

static inline cme_error_t
cmsc_decode_func_max_forwards(const struct cmsc_SipHeader *sip_header,
                              struct cmsc_SipMessage *msg) {
  msg->max_forwards = atoi(cmsc_bs_msg_to_string(&sip_header->value, msg).buf);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_MAX_FORWARDS);
  return 0;
};

static inline cme_error_t
cmsc_decode_func_via(const struct cmsc_SipHeader *sip_header,
                     struct cmsc_SipMessage *msg) {
  struct cmsc_SipHeaderVia *via = NULL;
  struct cmsc_ArgIterator iter;
  cme_error_t err;

  err = cmsc_arg_iterator_init(cmsc_bs_msg_to_string(&sip_header->value, msg),
                               &iter);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&iter))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      via = calloc(1, sizeof(struct cmsc_SipHeaderVia));
      if (!via) {
        err = cme_error(ENOMEM, "Cannot allocate memory for `via`");
        goto error_out;
      }

      const char *sent_by = iter.value.buf;
      const char *max = iter.value.buf + iter.value.len;
      const char *slash = NULL;
      while (sent_by != max) {
        if (*sent_by == '/') {
          slash = sent_by;
        } else if (*sent_by == ' ') {
          if (slash) {
            via->proto = cmsc_s_msg_to_bstring(
                &(struct cmsc_String){.buf = slash + 1,
                                      .len = sent_by - (slash + 1)},
                msg);
            via->sent_by = cmsc_s_msg_to_bstring(
                &(struct cmsc_String){.buf = sent_by + 1,
                                      .len = max - (sent_by + 1)},
                msg);
          }
          break;
        }
        sent_by++;
      }

      if (!via->proto.buf_offset || !via->sent_by.buf_offset) {
        free(via);
        err = cme_errorf(EINVAL, "Malformed Via sip header: %.*s",
                         sip_header->value.len,
                         cmsc_bs_msg_to_string(&sip_header->value, msg).buf);
        goto error_out;
      }

      STAILQ_INSERT_TAIL(&msg->vias, via, _next);
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_VIAS);
      break;
    }
    case cmsc_ArgNextResults_ARG: {
      if (!via) {
        break;
      }

      if (strncmp("addr", iter.arg_key.buf, iter.arg_key.len) == 0) {
        via->addr = cmsc_s_msg_to_bstring(&iter.arg_value, msg);
      } else if (strncmp("branch", iter.arg_key.buf, iter.arg_key.len) == 0) {
        via->branch = cmsc_s_msg_to_bstring(&iter.arg_value, msg);
      } else if (strncmp("received", iter.arg_key.buf, iter.arg_key.len) == 0) {
        via->received = cmsc_s_msg_to_bstring(&iter.arg_value, msg);
      } else if (strncmp("ttl", iter.arg_key.buf, iter.arg_key.len) == 0) {
        via->ttl = atoi(iter.arg_value.buf);
      }

      break;
    }
    default:;
    }
  }

  return 0;

error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsc_decode_func_content_length(const struct cmsc_SipHeader *sip_header,
                                struct cmsc_SipMessage *msg) {
  msg->content_length =
      atoi(cmsc_bs_msg_to_string(&sip_header->value, msg).buf);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CONTENT_LENGTH);
  return 0;
}

#endif

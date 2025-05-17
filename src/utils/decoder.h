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
cmsc_decode_func_via(const struct cmsc_SipHeader *sip_header,
                     struct cmsc_SipMessage *msg);
static inline cme_error_t
cmsc_decode_func_content_length(const struct cmsc_SipHeader *sip_header,
                                struct cmsc_SipMessage *msg);

static inline cme_error_t cmsc_decode_sip_headers(struct cmsc_SipMessage *msg) {
  static struct cmsc_DecoderLogic decoders[] = {
      {.header_id = {.buf = "To", .len = 2},
       .decode_func = cmsc_decode_func_to},
      {.header_id = {.buf = "Via", .len = 3},
       .decode_func = cmsc_decode_func_via},
      {.header_id = {.buf = "From", .len = 4},
       .decode_func = cmsc_decode_func_from},
      {.header_id = {.buf = "CSeq", .len = 4},
       .decode_func = cmsc_decode_func_cseq},
      {.header_id = {.buf = "Call-ID", .len = 7},
       .decode_func = cmsc_decode_func_call_id},
      {.header_id = {.buf = "Max-Forwards", .len = 11},
       .decode_func = cmsc_decode_func_max_forwards},
      {.header_id = {.buf = "Content-Length", .len = 11},
       .decode_func = cmsc_decode_func_content_length},
  };
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  struct cmsc_SipHeader *generic_header, *next_header;
  generic_header = STAILQ_FIRST(&msg->sip_headers);

  while (generic_header != NULL) {
    next_header = STAILQ_NEXT(generic_header, _next);

    while (
        isspace(*cmsc_bstring_dump_ptr(&generic_header->value, &msg->_buf))) {
      generic_header->value.buf_offset++;
      generic_header->value.len--;
    }

    while (isspace(cmsc_bstring_dump_ptr(
        &generic_header->value, &msg->_buf)[generic_header->value.len - 1])) {
      generic_header->value.len--;
    }

    // Parse generic header
    bool is_match = false;
    for (uint32_t i = 0;
         i < sizeof(decoders) / sizeof(struct cmsc_DecoderLogic); i++) {
      if (strncmp(decoders[i].header_id.buf,
                  cmsc_bstring_dump_ptr(&generic_header->key, &msg->_buf),
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
  struct cmsc_ArgIterator it;
  cme_error_t err;

  err = cmsc_arg_iterator_init(
      cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg), &it);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&it))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      while (*it.value.buf == '<') {
        it.value.buf++;
        it.value.len--;
      }
      while (it.value.buf[it.value.len - 1] == '>') {
        it.value.len--;
      }
      msg->to.uri = it.value;
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_TO);
      break;
    }
    case cmsc_ArgNextResults_ARG: {
      if (strncmp("tag", it.arg_key.buf, it.arg_key.len) == 0) {
        msg->to.tag = it.arg_value;
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
  struct cmsc_ArgIterator it;
  cme_error_t err;

  err = cmsc_arg_iterator_init(
      cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg), &it);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&it))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      while (*it.value.buf == '<') {
        it.value.buf++;
        it.value.len--;
      }
      while (it.value.buf[it.value.len - 1] == '>') {
        it.value.len--;
      }
      msg->from.uri = it.value;
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_FROM);
      break;
    }
    case cmsc_ArgNextResults_ARG: {
      if (strncmp("tag", it.arg_key.buf, it.arg_key.len) == 0) {
        msg->from.tag = it.arg_value;
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
      cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg).buf;
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
  msg->cseq.method.buf = method;
  msg->cseq.method.len = end - method;
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CSEQ);
  return 0;
}

static inline cme_error_t
cmsc_decode_func_call_id(const struct cmsc_SipHeader *sip_header,
                         struct cmsc_SipMessage *msg) {
  msg->call_id = cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CALL_ID);
  return 0;
};

static inline cme_error_t
cmsc_decode_func_max_forwards(const struct cmsc_SipHeader *sip_header,
                              struct cmsc_SipMessage *msg) {
  msg->max_forwards =
      atoi(cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg).buf);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_MAX_FORWARDS);
  return 0;
};

static inline cme_error_t
cmsc_decode_func_via(const struct cmsc_SipHeader *sip_header,
                     struct cmsc_SipMessage *msg) {
  struct cmsc_SipHeaderVia *via = NULL;
  struct cmsc_ArgIterator it;
  cme_error_t err;

  err = cmsc_arg_iterator_init(
      cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg), &it);
  if (err) {
    goto error_out;
  }

  enum cmsc_ArgNextResults result;

  while ((result = cmsc_arg_iterator_next(&it))) {
    switch (result) {
    case cmsc_ArgNextResults_VALUE: {
      via = calloc(1, sizeof(struct cmsc_SipHeaderVia));
      if (!via) {
        err = cme_error(ENOMEM, "Cannot allocate memory for `via`");
        goto error_out;
      }

      const char *sent_by = it.value.buf;
      const char *max = it.value.buf + it.value.len;
      const char *slash = NULL;
      while (sent_by != max) {
        if (*sent_by == '/') {
          slash = sent_by;
        } else if (*sent_by == ' ') {
          if (slash) {
            via->proto.buf = slash + 1;
            via->proto.len = sent_by - via->proto.buf;
            via->sent_by.buf = sent_by + 1;
            via->sent_by.len = max - via->sent_by.buf;
          }
          break;
        }
        sent_by++;
      }

      if (!via->proto.buf || !via->sent_by.buf) {
        free(via);
        /* err = cme_errorf(EINVAL, "Malformed Via sip header: %.*s", */
        /*                  sip_header->value.len, sip_header->value.buf); */
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

      if (strncmp("addr", it.arg_key.buf, it.arg_key.len) == 0) {
        via->addr = it.arg_value;
      } else if (strncmp("branch", it.arg_key.buf, it.arg_key.len) == 0) {
        via->branch = it.arg_value;
      } else if (strncmp("received", it.arg_key.buf, it.arg_key.len) == 0) {
        via->received = it.arg_value;
      } else if (strncmp("ttl", it.arg_key.buf, it.arg_key.len) == 0) {
        via->ttl = atoi(it.arg_value.buf);
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
      atoi(cmsc_sipmsg_bstring_dump_string(&sip_header->value, msg).buf);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CONTENT_LENGTH);
  return 0;
}

#endif

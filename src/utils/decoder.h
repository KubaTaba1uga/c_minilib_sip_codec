/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_DECODER_H
#define C_MINILIB_SIP_CODEC_DECODER_H

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
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

static inline cme_error_t cmsc_decode_sip_headers(struct cmsc_SipMessage *msg) {
  static struct cmsc_DecoderLogic decoders[] = {
      {.header_id = {.buf = "To", .len = 2},
       .decode_func = cmsc_decode_func_to},
      {.header_id = {.buf = "From", .len = 4},
       .decode_func = cmsc_decode_func_from},
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

    // Parse generic header
    bool is_match = false;
    for (uint32_t i = 0;
         i < sizeof(decoders) / sizeof(struct cmsc_DecoderLogic); i++) {
      if (strncmp(decoders[i].header_id.buf, generic_header->key.buf,
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

  err = cmsc_arg_iterator_init(sip_header->value, &it);
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

  err = cmsc_arg_iterator_init(sip_header->value, &it);
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

#endif

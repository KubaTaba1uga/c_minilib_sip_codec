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

// We need to assign fields to 2^N values, to represent seperate
//  bits in `present_mask` field.
enum cmsc_SipField {
  cmsc_SipField_SIP_PROTO_VER = 1,
  cmsc_SipField_IS_REQUEST = 2,
  cmsc_SipField_TO = 4,
  cmsc_SipField_FROM = 8,
  cmsc_SipField_CSEQ = 16,
};

struct cmsc_SipMsg {
  uint32_t presence_mask;
  bool is_request;
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

#endif

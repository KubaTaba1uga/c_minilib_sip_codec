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

#ifndef CMSC_SIPMSG_DEFAULT_BUF_SIZE
#define CMSC_SIPMSG_DEFAULT_BUF_SIZE 512
#endif

// This function assumes user keeps ownership over _buf memory
static inline cme_error_t cmsc_sipmsg_create(struct cmsc_Buffer buf,
                                             struct cmsc_SipMessage **msg) {
  struct cmsc_SipMessage *local_msg;
  cme_error_t err;

  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  local_msg = calloc(1, sizeof(struct cmsc_SipMessage));
  if (!local_msg) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_msg`");
    goto error_out;
  }

  STAILQ_INIT(&local_msg->sip_headers);
  STAILQ_INIT(&local_msg->vias);

  local_msg->_buf = buf;
  *msg = local_msg;

  return 0;

error_out:
  return cme_return(err);
};

static inline void
cmsc_sipmsg_mark_field_present(struct cmsc_SipMessage *msg,
                               enum cmsc_SupportedSipHeaders header_id) {
  msg->presence_mask = msg->presence_mask | header_id;
}

#endif

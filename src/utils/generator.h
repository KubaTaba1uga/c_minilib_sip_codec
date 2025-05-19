/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_GENERATOR_H
#define C_MINILIB_SIP_CODEC_GENERATOR_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/encoder.h"
#include "utils/sipmsg.h"

static inline cme_error_t
cmsc_generate_first_line(const struct cmsc_SipMessage *msg,
                         struct cmsc_Buffer *buf) {
  cme_error_t err;
  if (cmsc_sipmsg_is_field_present((struct cmsc_SipMessage *)msg,
                                   cmsc_SupportedSipHeaders_REQUEST_LINE)) {
    err = cmsc_encode_request_line(msg, buf);
    if (err) {
      goto error_out;
    }
  } else if (cmsc_sipmsg_is_field_present(
                 (struct cmsc_SipMessage *)msg,
                 cmsc_SupportedSipHeaders_STATUS_LINE)) {
    err = cmsc_encode_status_line(msg, buf);
    if (err) {
      goto error_out;
    }
  } else {
    err =
        cme_error(EINVAL, "Cannot generate msg without request or status line");
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t
cmsc_generate_generic_headers(const struct cmsc_SipMessage *msg,
                              struct cmsc_Buffer *buf) {

  cme_error_t err;
  struct cmsc_SipHeader *hdr;
  STAILQ_FOREACH(hdr, &msg->sip_headers, _next) {
    err = cmsc_encode_generic_hdr(msg, hdr, buf);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif

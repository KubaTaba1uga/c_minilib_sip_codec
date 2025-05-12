/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_SUPPORTED_MSG_H
#define C_MINILIB_SIP_CODEC_SUPPORTED_MSG_H
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/line_iterator.h"
#include "sipmsg/sipmsg.h"

static inline cme_error_t
cmsc_parser_parse_supported_msg(const uint32_t buffer_len, const char *buffer,
                                cmsc_sipmsg_t msg,
                                const char **supported_msg_id_ptr) {
  const char *supported_msg_id = NULL;
  cme_error_t err;

  CMSC_SUPPORTED_MESSAGES_FOREACH(i) {
    if ((supported_msg_id = cmsc_strnstr(
             buffer, cmsc_dump_supported_messages_string(i), buffer_len))) {
      cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_SUPPORTED_MSG);
      msg->supmsg = i;
      break;
    }
  }

  if (supported_msg_id_ptr) {
    *supported_msg_id_ptr = supported_msg_id;
  }

  if (!supported_msg_id) {

    err = cme_error(EINVAL, "Unsupported SIP message");
    goto error_out;
  }

  return NULL;
error_out:
  return cme_return(err);
};

#endif

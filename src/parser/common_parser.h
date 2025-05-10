/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_COMMON_PARSER_H
#define C_MINILIB_SIP_CODEC_COMMON_PARSER_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

static inline cme_error_t
cmsc_parser_parse_sip_proto_ver(const uint32_t buffer_len, const char *buffer,
                                cmsc_sipmsg_t msg) {
  cme_error_t err;

  const char *slash = cmsc_strnstr(buffer, "/", buffer_len);
  if (!slash) {
    err = cme_error(EINVAL, "Invalid SIP protocol version, major number");
    goto error_out;
  }

  char mini_buf[2];
  mini_buf[0] = *(++slash);
  mini_buf[1] = 0;

  msg->sip_proto_ver.major = atoi(mini_buf);

  const char *dot = cmsc_strnstr(slash, ".", buffer_len - (slash - buffer));
  if (!dot) {
    err = cme_error(EINVAL, "Invalid SIP protocol version, minor number");
    goto error_out;
  }

  mini_buf[0] = *(++dot);
  mini_buf[1] = 0;
  msg->sip_proto_ver.minor = atoi(mini_buf);

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_SIP_PROTO_VER);

  return NULL;
error_out:
  return cme_return(err);
};

#endif

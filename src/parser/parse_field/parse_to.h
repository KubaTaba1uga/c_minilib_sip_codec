/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_TO_H
#define C_MINILIB_SIP_CODEC_PARSE_TO_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

static inline cme_error_t cmsc_parser_parse_to(const uint32_t buffer_len,
                                               const char *buffer,
                                               cmsc_sipmsg_t msg) {
  /* cme_error_t err; */
  puts("Parsing TO");

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_TO);
  msg->to.uri = cmsc_sipmsg_insert_str(buffer_len, buffer, &msg);

  return 0;
  /* error_out: */
  /* return cme_return(err); */
};

#endif

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include <asm-generic/errno-base.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/common_parser.h"
#include "parser/iterator/line_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "parser/parse_field/parse_request_line.h"
#include "parser/parse_field/parse_sip_proto_ver.h"
#include "parser/parse_field/parse_status_line.h"
#include "parser/parse_field/parse_supported_msg.h"
#include "scheme/scheme.h"
#include "scheme/scheme_register.h"
#include "sipmsg/sipmsg.h"
#include "utils/dynamic_buffer.h"
#include "utils/string.h"

struct cmsc_Parser {
  enum cmsc_ParserStates state;
  cmsc_sipmsg_t msg;
  struct cmsc_DynamicBuffer content;
};

static inline cmsc_sipmsg_t
cmsc_parser_parse_first_line(struct cmsc_Line *line) {
  cmsc_sipmsg_t sipmsg;
  cme_error_t err;

  const char *sip_version = NULL;
  const char *supported_msg_id = NULL;

  sip_version = cmsc_strnstr(line->start, "SIP/", line->len);
  if (!sip_version) {
    goto error_out;
  }

  if ((err = cmsc_sipmsg_create(&sipmsg))) {
    goto error_out;
  }

  if ((err = cmsc_parser_parse_supported_msg(line->len, line->start, &sipmsg,
                                             &supported_msg_id))) {
    goto error_sipmsg_cleanup;
  }

  if (sip_version > supported_msg_id) {
    if ((err =
             cmsc_parser_parse_request_line(line->len, line->start, &sipmsg))) {
      goto error_sipmsg_cleanup;
    }
  } else {
    if ((err =
             cmsc_parser_parse_status_line(line->len, line->start, &sipmsg))) {
      goto error_sipmsg_cleanup;
    }
  }

  return sipmsg;

error_sipmsg_cleanup:
  cmsc_sipmsg_destroy(&sipmsg);
error_out:
  return NULL;
}

#endif

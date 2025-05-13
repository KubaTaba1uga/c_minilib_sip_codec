/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_CSEQ_H
#define C_MINILIB_SIP_CODEC_PARSE_CSEQ_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline cme_error_t
cmsc_parser_parse_cseq(const struct cmsc_ValueLine *line, cmsc_sipmsg_t *msg) {
  const char *method = line->value.start;

  while (isdigit(*method) || isspace(*method)) {
    if (method == line->value.end) {
      break;
    }
    method++;
  }

  (*msg)->cseq.seq_number = atoi(line->value.start);

  (*msg)->cseq.method =
      cmsc_sipmsg_insert_str(line->value.end - method, method, msg);

  cmsc_sipmsg_mark_field_present(*msg, cmsc_SupportedFields_CSEQ);

  return 0;
}

#endif

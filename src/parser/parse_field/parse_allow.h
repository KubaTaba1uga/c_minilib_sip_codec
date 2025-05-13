/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_ALLOW_H
#define C_MINILIB_SIP_CODEC_PARSE_ALLOW_H

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
#include <string.h>

static inline cme_error_t cmsc_parser_parse_allow(struct cmsc_ValueLine *line,
                                                  cmsc_sipmsg_t *msg) {
  CMSC_SUPPORTED_MESSAGES_FOREACH(supmsg) {
    if (cmsc_strnstr(line->value.start,
                     cmsc_dump_supported_messages_string(supmsg),
                     line->value.len)) {
      (*msg)->allow_mask |= supmsg;
      break;
    }
  }

  if ((*msg)->allow_mask) {
    cmsc_sipmsg_mark_field_present((*msg), cmsc_SupportedFields_ALLOW);
  }

  return 0;
}

#endif

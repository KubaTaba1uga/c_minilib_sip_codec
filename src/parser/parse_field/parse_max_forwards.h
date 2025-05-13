/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_MAX_FORWARDS_H
#define C_MINILIB_SIP_CODEC_PARSE_MAX_FORWARDS_H

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
cmsc_parser_parse_max_forwards(const struct cmsc_ValueLine *value_line,
                               cmsc_sipmsg_t *msg) {
  (*msg)->max_forwards = atoi(value_line->value.start);
  cmsc_sipmsg_mark_field_present((*msg), cmsc_SupportedFields_MAX_FORWARDS);

  return 0;
}

#endif

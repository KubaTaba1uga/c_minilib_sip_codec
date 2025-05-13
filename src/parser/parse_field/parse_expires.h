/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_EXPIRES_H
#define C_MINILIB_SIP_CODEC_PARSE_EXPIRES_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline cme_error_t
cmsc_parser_parse_expires(const struct cmsc_ValueLine *line,
                          cmsc_sipmsg_t *msg) {
  (*msg)->expires = atoi(line->value.start);

  cmsc_sipmsg_mark_field_present((*msg), cmsc_SupportedFields_EXPIRES);

  return 0;
}

#endif

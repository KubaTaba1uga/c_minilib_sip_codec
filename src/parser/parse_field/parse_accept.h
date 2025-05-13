/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_ACCEPT_H
#define C_MINILIB_SIP_CODEC_PARSE_ACCEPT_H

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

static inline cme_error_t
cmsc_parser_parse_accept(const struct cmsc_ValueLine *line,
                         cmsc_sipmsg_t *msg) {
  struct cmsc_Field_Accept *accept =
      calloc(1, sizeof(struct cmsc_Field_Accept));

  accept->mime =
      cmsc_sipmsg_insert_str(line->value.len, line->value.start, msg);

  STAILQ_INSERT_TAIL(&(*msg)->accept, accept, accepts_l);

  cmsc_sipmsg_mark_field_present((*msg), cmsc_SupportedFields_ACCEPT);

  return NULL;
}

#endif

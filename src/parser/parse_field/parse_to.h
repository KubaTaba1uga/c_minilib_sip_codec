/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_TO_H
#define C_MINILIB_SIP_CODEC_PARSE_TO_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

static inline cme_error_t
cmsc_parser_parse_to(const struct cmsc_ValueIterator *value_iter,
                     cmsc_sipmsg_t msg) {
  struct cmsc_ArgsIterator args_iter;
  cme_error_t err;

  if ((err = cmsc_argsiter_init(&args_iter))) {
    goto error_out;
  }

  uint32_t i = 0;
  while (cmsc_argsiter_next(value_iter, &args_iter)) {
    if (i++ == 0) {
      msg->to.uri =
          cmsc_sipmsg_insert_str(args_iter.value_len, args_iter.value, &msg);
    }

    if (args_iter.args_header) {
      if (cmsc_strnstr(args_iter.args_header, "tag",
                       args_iter.args_header_len)) {
        msg->to.tag = cmsc_sipmsg_insert_str(args_iter.args_value_len,
                                             args_iter.args_value, &msg);
      }
    }
  }

  if (i > 0) {
    cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_TO);
  }

  return 0;

error_out:
  return cme_return(err);
};

#endif

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_FROM_H
#define C_MINILIB_SIP_CODEC_PARSE_FROM_H

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
cmsc_parser_parse_from(const struct cmsc_ValueIterator *value_iter,
                       cmsc_sipmsg_t msg) {
  if (!value_iter->value_start) {
    return 0;
  }

  struct cmsc_ArgsIterator args_iter;
  cme_error_t err;

  if ((err = cmsc_argsiter_init(&args_iter))) {
    goto error_out;
  }

  uint32_t i = 0;
  while (cmsc_argsiter_next(value_iter, &args_iter)) {
    if (!args_iter.value || !args_iter.value_len) {
      break;
    }

    if (i++ == 0) {
      const char *display_name_end_cp, *display_name_start = args_iter.value;
      const char *display_name_end = display_name_end_cp =
          cmsc_strnstr(args_iter.value, "<", args_iter.value_len);
      if (display_name_end) {
        while (isspace(*display_name_start)) {
          display_name_start++;
        }

        while (isspace(*(display_name_end - 1))) {
          display_name_end--;
        }

        if (display_name_start < display_name_end) {
          msg->from.display_name = cmsc_sipmsg_insert_str(
              display_name_end - display_name_start, display_name_start, &msg);

          args_iter.value = display_name_end_cp;
          args_iter.value_len -= (display_name_end_cp - display_name_start);
        }
      }

      msg->from.uri =
          cmsc_sipmsg_insert_str(args_iter.value_len, args_iter.value, &msg);
    }

    if (args_iter.args_header) {
      if (cmsc_strnstr(args_iter.args_header, "tag",
                       args_iter.args_header_len)) {
        msg->from.tag = cmsc_sipmsg_insert_str(args_iter.args_value_len,
                                               args_iter.args_value, &msg);
      }
    }
  }

  if (i > 0) {
    cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_FROM);
  }

  return 0;

error_out:
  return cme_return(err);
};

#endif

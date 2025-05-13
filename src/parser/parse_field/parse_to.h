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

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline cme_error_t
cmsc_parser_parse_to(const struct cmsc_ValueLine *line, cmsc_sipmsg_t *msg) {
  struct cmsc_ArgsIterator args_iter;
  cme_error_t err;

  if ((err = cmsc_args_iterator_init(line, &args_iter))) {
    goto error_out;
  }

  struct cmsc_ArgsLine args = {0};
  bool do_loop = true;
  while (do_loop) {
    enum cmsc_ArgsNextResults result =
        cmsc_args_iterator_next(&args, &args_iter);
    switch (result) {
    case cmsc_ArgsNextResults_NONE:
      do_loop = false;
      break;
    case cmsc_ArgsNextResults_VALUE: {
      const char *display_name_end =
          cmsc_strnstr(args.value.start, "<", args.value.len);

      if (display_name_end == args.value.start) {
        (*msg)->to.uri =
            cmsc_sipmsg_insert_str(args.value.len, args.value.start, msg);

      } else {
        (*msg)->to.uri = cmsc_sipmsg_insert_str(
            args.value.end - display_name_end, display_name_end, msg);

        display_name_end--;
        (*msg)->to.display_name = cmsc_sipmsg_insert_str(
            display_name_end - args.value.start, args.value.start, msg);
      }

      break;
    }
    case cmsc_ArgsNextResults_ARG:
      if (strncmp(args.arg_key.start, "tag", args.arg_key.len) == 0) {
        (*msg)->to.tag = cmsc_sipmsg_insert_str(args.arg_value.len,
                                                args.arg_value.start, msg);
      }
      break;
    }
  }

  if ((*msg)->to.uri) {
    cmsc_sipmsg_mark_field_present(*msg, cmsc_SupportedFields_TO);
  }

  return 0;

error_out:
  return cme_return(err);
};

#endif

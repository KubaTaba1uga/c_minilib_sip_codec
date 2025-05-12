/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_VIA_L_H
#define C_MINILIB_SIP_CODEC_PARSE_VIA_L_H

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "parser/iterator/args_iterator.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"
#include "utils/string.h"

#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

static inline cme_error_t
cmsc_parser_parse_via_l(const struct cmsc_HeaderIterator *header_iter,
                        struct cmsc_ValueIterator *value_iter,
                        cmsc_sipmsg_t msg) {
  puts("Parsing argsiter");

  STAILQ_INIT(&msg->via);

  struct cmsc_ArgsIterator args_iter;
  cme_error_t err;

  if ((err = cmsc_argsiter_init(&args_iter))) {
    goto error_out;
  }

  do {
    struct cmsc_Field_Via *via = calloc(1, sizeof(struct cmsc_Field_Via));
    STAILQ_INSERT_TAIL(&msg->via, via, vias_l);

    uint32_t i = 0;
    while (cmsc_argsiter_next(value_iter, &args_iter)) {
      if (!args_iter.value || !args_iter.value_len) {
        break;
      }

      if (i == 0) {
        const char *trans_proto = NULL;
        for (int proto = cmsc_TransportProtocols_NONE + 1;
             proto < cmsc_TransportProtocols_MAX; proto++) {

          if ((trans_proto = cmsc_strnstr(args_iter.value,
                                          cmsc_sipmsg_dump_transp_proto(proto),
                                          args_iter.value_len))) {
            via->transp_proto = proto;
            break;
          }
        }

        if (!trans_proto) {
          err = cme_error(EINVAL, "No transport protocol in via field");
          goto error_out;
        }

        while (isalnum(*trans_proto)) {
          trans_proto++;
        }
        while (isspace(*trans_proto)) {
          trans_proto++;
        }

        via->sent_by = cmsc_sipmsg_insert_str(
            (args_iter.value + args_iter.value_len) - trans_proto, trans_proto,
            &msg);
      }
      i++;

      if (args_iter.args_header) {

        if (cmsc_strnstr(args_iter.args_header, "branch",
                         args_iter.args_header_len)) {
          via->branch = cmsc_sipmsg_insert_str(args_iter.args_value_len,
                                               args_iter.args_value, &msg);
          printf("Parsed branch: %.*s\n", args_iter.args_value_len,
                 args_iter.args_value);
        }

        if (cmsc_strnstr(args_iter.args_header, "addr",
                         args_iter.args_header_len)) {
          via->addr = cmsc_sipmsg_insert_str(args_iter.args_value_len,
                                             args_iter.args_value, &msg);
          printf("Parsed addr: %.*s\n", args_iter.args_value_len,
                 args_iter.args_value);
        }
      }
    }

  } while (cmsc_valueiter_next(header_iter, value_iter));

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedFields_VIA_L);

  return 0;

error_out:
  return cme_return(err);
}

#endif

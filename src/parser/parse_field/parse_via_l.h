/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSE_VIA_H
#define C_MINILIB_SIP_CODEC_PARSE_VIA_H

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

/* "Via: SIP/2.0/UDP sip.test.org;branch=z9hG4bK-123;addr=10.0.0.1\r\n"; */
static inline cme_error_t
cmsc_parser_parse_via(const struct cmsc_ValueLine *line, cmsc_sipmsg_t *msg) {
  struct cmsc_ArgsIterator args_iter;
  cme_error_t err;

  if ((err = cmsc_args_iterator_init(line, &args_iter))) {
    goto error_out;
  }

  struct cmsc_Field_Via *via = calloc(1, sizeof(struct cmsc_Field_Via));

  struct cmsc_ArgsLine args = {0};
  bool do_loop = true;
  while (do_loop) {
    switch (cmsc_args_iterator_next(&args, &args_iter)) {
    case cmsc_ArgsNextResults_NONE: {
      do_loop = false;
      break;
    }
    case cmsc_ArgsNextResults_VALUE: {
      const char *trans_proto = NULL;
      for (int proto = cmsc_TransportProtocols_NONE + 1;
           proto < cmsc_TransportProtocols_MAX; proto++) {
        if ((trans_proto = cmsc_strnstr(line->value.start,
                                        cmsc_sipmsg_dump_transp_proto(proto),
                                        line->value.len))) {
          via->transp_proto = proto;
          break;
        }
      }

      if (!trans_proto) {
        err = cme_error(EINVAL, "No transport protocol in via field");
        goto error_via_cleanup;
      }

      while (isalnum(*trans_proto)) {
        trans_proto++;
      }
      while (isspace(*trans_proto)) {
        trans_proto++;
      }

      via->sent_by = cmsc_sipmsg_insert_str(args.value.end - trans_proto,
                                            trans_proto, msg);

      break;
    }
    case cmsc_ArgsNextResults_ARG: {
      if (cmsc_strnstr(args.arg_key.start, "branch", args.arg_key.len)) {
        via->branch = cmsc_sipmsg_insert_str(args.arg_value.len,
                                             args.arg_value.start, msg);
      } else if (cmsc_strnstr(args.arg_key.start, "addr", args.arg_key.len)) {
        via->addr = cmsc_sipmsg_insert_str(args.arg_value.len,
                                           args.arg_value.start, msg);
      } else if (cmsc_strnstr(args.arg_key.start, "received",
                              args.arg_key.len)) {
        via->received = cmsc_sipmsg_insert_str(args.arg_value.len,
                                               args.arg_value.start, msg);
      } else if (cmsc_strnstr(args.arg_key.start, "ttl", args.arg_key.len)) {
        via->ttl = atoi(args.arg_value.start);
      }

      break;
    }
    }
  }

  cmsc_sipmsg_mark_field_present(*msg, cmsc_SupportedFields_VIA_L);
  STAILQ_INSERT_TAIL(&(*msg)->via, via, vias_l);

  return 0;

error_via_cleanup:
  free(via);
error_out:
  return cme_return(err);
};

#endif

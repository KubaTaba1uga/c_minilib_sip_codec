/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/bstring.h"
#include "utils/buffer.h"
#include "utils/encoder.h"
#include "utils/generator.h"

#ifndef CMSC_GENERATOR_DEFAULT_SPACE_SIZE
#define CMSC_GENERATOR_DEFAULT_SPACE_SIZE 128
#endif

cme_error_t cmsc_generate_sip(const struct cmsc_SipMessage *msg,
                              uint32_t *buf_len, const char **buf) {
  // TO-DO: validate msg content befor generation
  cme_error_t err;
  if (!msg || !buf_len || !buf) {
    err = cme_error(EINVAL, "`msg`, `buf_len` and `buf` cannot be NULL");
    goto error_out;
  }

  struct cmsc_Buffer local_buf = {
      .len = 0,
      .size = msg->_buf.len + CMSC_GENERATOR_DEFAULT_SPACE_SIZE,
      .buf = calloc(msg->_buf.len + CMSC_GENERATOR_DEFAULT_SPACE_SIZE,
                    sizeof(char))};
  if (!local_buf.buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_buf`");
    goto error_out;
  }

  err = cmsc_generate_first_line(msg, &local_buf);
  if (err) {
    goto error_local_buf_cleanup;
  }

  err = cmsc_encode_sip_headers(msg, &local_buf);
  if (err) {
    goto error_local_buf_cleanup;
  }

  err = cmsc_generate_generic_headers(msg, &local_buf);
  if (err) {
    goto error_local_buf_cleanup;
  }

  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = "\r\n", .len = strlen("\r\n")}, &local_buf,
      NULL);
  if (err) {
    goto error_local_buf_cleanup;
  }

  *buf = local_buf.buf;
  *buf_len = local_buf.len;

  return 0;

error_local_buf_cleanup:
  free((void *)local_buf.buf);
error_out:
  return cme_return(err);
};

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_PARSER_H
#define C_MINILIB_SIP_CODEC_PARSER_H

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

static inline cme_error_t cmsc_parse_sip_headers(struct cmsc_SipMessage *msg) {
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  const char *max_char = msg->_buf.buf + msg->_buf.len;
  const char *current_char = msg->_buf.buf;
  const char *line_start = msg->_buf.buf;
  struct cmsc_SipHeader *header = NULL;
  uint32_t clrf_counter = 0;

  while (current_char != max_char) {
    switch (*current_char) {
    case ':': {
      if (!header) {
        header = calloc(1, sizeof(struct cmsc_SipHeader));
        if (!header) {
          err = cme_error(ENOMEM, "Cannot allocate memory for `header`");
          goto error_out;
        }

        header->key.buf = line_start;
        header->key.len = current_char - line_start;
        clrf_counter = 0;
      }
      break;
    }
    case '\n': {
      if (*(current_char - 1) == '\r') {
        if (header) {
          header->value.buf = header->key.buf + header->key.len + 1;
          header->value.len = (current_char - header->value.buf) - 1;
          STAILQ_INSERT_TAIL(&msg->sip_headers, header, _next);
          header = NULL;
        }

        clrf_counter++;
        line_start = current_char + 1;
      }
      break;
    }
    }

    current_char++;
  }

  // If not full line parsed we need to clean it up
  if (clrf_counter == 0 && header) {
    free(header);
  }

  (void)clrf_counter;

  return 0;

error_out:
  return cme_return(err);
}

static inline cme_error_t cmsc_parse_request_line(const uint32_t buffer_len,
                                                  const char *buf,
                                                  struct cmsc_SipMessage *msg) {
  /*
    According RFC 3261 25 request line looks like this:
      Method SP Request-URI SP SIP-Version CRLF
  */
  char buffer[buffer_len + 1];
  cme_error_t err;

  memcpy(buffer, buf, buffer_len);
  buffer[buffer_len] = 0;

  char *method_end = (char *)buffer;
  uint32_t method_len = 0;

  while (isalpha(*method_end)) {
    if (method_len >= buffer_len) {
      break;
    }
    method_len++;
    method_end++;
  }

  if (!isspace(*method_end)) {
    err = cme_error(EINVAL, "No space after method in request line");
    goto error_out;
  }

  char *sip_version = strstr(method_end, "SIP/");
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version in request line");
    goto error_out;
  }
  uint32_t sip_version_len = (buffer + buffer_len) - sip_version;

  const char *request_uri = method_end++;
  while (isspace(*request_uri)) {
    request_uri++;
  }

  uint32_t request_uri_len = 0;
  while (!isspace(request_uri[request_uri_len])) {
    request_uri_len++;
  }

  msg->request_line.sip_method.buf = buf;
  msg->request_line.sip_method.len = method_len;

  msg->request_line.sip_proto_ver.buf = buf + (sip_version - buffer);
  msg->request_line.sip_proto_ver.len = sip_version_len;

  msg->request_line.request_uri.buf = buf + (request_uri - buffer);
  msg->request_line.request_uri.len = request_uri_len;

  return NULL;
error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsc_parse_sip_first_line(struct cmsc_SipMessage *msg) {
  /*
    According RFC 3261 25 request line looks like this:
      Method SP Request-URI SP SIP-Version CRLF
    and status line like this:
      SIP-Version SP Status-Code SP Reason-Phrase CRLF
  */
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  const char *line_end = strstr(msg->_buf.buf, "\r\n");
  if (!line_end) {
    err = cme_error(EINVAL, "No CLRF");
    goto error_out;
  }

  const char *sip_version = strstr(msg->_buf.buf, "SIP/");
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version");
    goto error_out;
  }

  if (sip_version > line_end) {
    err = cme_error(EINVAL, "Sip version has to be in first line");
    goto error_out;
  }

  if (sip_version == msg->_buf.buf) {

  } else {
    err = cmsc_parse_request_line(line_end - msg->_buf.buf, msg->_buf.buf, msg);
    if (err) {
      goto error_out;
    }
  }

  return 0;

error_out:
  return cme_return(err);
}

#endif

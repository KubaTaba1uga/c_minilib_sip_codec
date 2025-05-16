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

static inline cme_error_t cmsc_parse_sip_headers(const struct cmsc_Buffer *buf,
                                                 struct cmsc_SipMessage *msg) {
  cme_error_t err;
  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  const char *max_char = buf->buf + buf->len;
  const char *current_char = buf->buf;
  const char *line_start = buf->buf;
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

static inline cme_error_t cmsc_parse_request_line(const struct cmsc_Buffer *buf,
                                                  struct cmsc_SipMessage *msg) {
  /*
    According RFC 3261 25 request line looks like this:
      Method SP Request-URI SP SIP-Version CRLF
  */
  char buffer[buf->len + 1];
  cme_error_t err;

  memcpy(buffer, buf->buf, buf->len);
  buffer[buf->len] = 0;

  char *method_end = (char *)buffer;
  uint32_t method_len = 0;

  while (isalpha(*method_end)) {
    if (method_len >= buf->len) {
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
  uint32_t sip_version_len = (buffer + buf->len) - sip_version;

  const char *request_uri = method_end++;
  while (isspace(*request_uri)) {
    request_uri++;
  }

  uint32_t request_uri_len = 0;
  while (!isspace(request_uri[request_uri_len])) {
    request_uri_len++;
  }

  msg->request_line.sip_method.buf = buf->buf;
  msg->request_line.sip_method.len = method_len;

  msg->request_line.sip_proto_ver.buf = buf->buf + (sip_version - buffer);
  msg->request_line.sip_proto_ver.len = sip_version_len;

  msg->request_line.request_uri.buf = buf->buf + (request_uri - buffer);
  msg->request_line.request_uri.len = request_uri_len;

  return NULL;
error_out:
  return cme_return(err);
};

static inline cme_error_t cmsc_parse_status_line(const struct cmsc_Buffer *buf,
                                                 struct cmsc_SipMessage *msg) {
  /*
    According RFC 3261 25 status line looks like this:
      SIP-Version SP Status-Code SP Reason-Phrase CRLF
  */
  char buffer[buf->len + 1];
  cme_error_t err;

  memcpy(buffer, buf->buf, buf->len);
  buffer[buf->len] = 0;

  char *sip_version = (char *)buffer;
  char *space = strstr(sip_version, " ");
  if (!space) {
    err = cme_error(EINVAL, "No sip version in status line");
    goto error_out;
  }
  uint32_t sip_version_len = space - sip_version;

  const char *status_code = space + 1;
  space = strstr(status_code, " ");
  if (!space) {
    err = cme_error(EINVAL, "No status code in status line");
    goto error_out;
  }

  const char *reason_phrase = space + 1;
  uint32_t reason_phrase_len = (buffer + buf->len) - reason_phrase;

  uint32_t code = atoi(status_code);
  if (!code) {
    err = cme_error(EINVAL, "Malformed status code in status line");
    goto error_out;
  }

  msg->status_line.sip_proto_ver.buf = buf->buf;
  msg->status_line.sip_proto_ver.len = sip_version_len;

  msg->status_line.status_code = code;

  msg->status_line.reason_phrase.buf = buf->buf + (reason_phrase - buffer);
  msg->status_line.reason_phrase.len = reason_phrase_len;

  return NULL;
error_out:
  return cme_return(err);
};

static inline cme_error_t
cmsc_parse_sip_first_line(struct cmsc_Buffer *buf,
                          struct cmsc_SipMessage *msg) {
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

  const char *line_max = strstr(buf->buf, "\r\n");
  if (!line_max) {
    err = cme_error(EINVAL, "No CLRF");
    goto error_out;
  }

  const char *sip_version = strstr(buf->buf, "SIP/");
  if (!sip_version) {
    err = cme_error(EINVAL, "No sip version");
    goto error_out;
  }

  if (sip_version > line_max) {
    err = cme_error(EINVAL, "Sip version has to be in first line");
    goto error_out;
  }

  if (sip_version == buf->buf) {
    err = cmsc_parse_status_line(
        &(struct cmsc_Buffer){.buf = buf->buf,
                              .size = line_max - buf->buf,
                              .len = line_max - buf->buf},
        msg);
    if (err) {
      goto error_out;
    }
  } else {
    err = cmsc_parse_request_line(
        &(struct cmsc_Buffer){.buf = buf->buf,
                              .size = line_max - buf->buf,
                              .len = line_max - buf->buf},
        msg);
    if (err) {
      goto error_out;
    }
  }

  uint32_t first_line_offset = (line_max - buf->buf) + strlen("\r\n");
  buf->buf += first_line_offset;
  buf->len -= first_line_offset;

  return 0;

error_out:
  return cme_return(err);
}

#endif

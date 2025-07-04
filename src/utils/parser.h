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
#include "utils/bstring.h"
#include "utils/siphdr.h"
#include "utils/sipmsg.h"

static inline cme_error_t cmsc_parse_sip_headers(struct cmsc_Buffer *buf,
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
        err = cmsc_siphdr_create(current_char - line_start,
                                 line_start - msg->_buf.buf, 0, 0, &header);
        if (err) {
          goto error_out;
        }
        clrf_counter = 0;
      }
      break;
    }
    case '\n': {
      if (*(current_char - 1) == '\r') {
        clrf_counter++;

        const char *old_line_start = line_start;
        line_start = current_char + 1;
        if (clrf_counter == 2 &&
            (current_char - old_line_start) <= 2) { // We hit the body
          current_char = max_char - 1;              // So let's stop the loop
          break;
        }

        if (header) {
          header->value.buf_offset =
              header->key.buf_offset + header->key.len + 1;
          header->value.len =
              (current_char - (msg->_buf.buf + header->value.buf_offset)) - 1;
          STAILQ_INSERT_TAIL(&msg->sip_headers, header, _next);

          header = NULL;
        }
      }
      break;
    }
    default:;
    }

    current_char++;
  }

  // If not full line parsed we need to clean it up
  if (clrf_counter == 0 && header) {
    free(header);
  }

  uint32_t body_offset = (line_start - buf->buf);
  buf->buf += body_offset;
  buf->len -= body_offset;

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

  msg->request_line.sip_method = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.buf = buf->buf, .len = method_len}, msg);

  msg->request_line.sip_proto_ver = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.buf = buf->buf + (sip_version - buffer),
                            .len = sip_version_len},
      msg);

  msg->request_line.request_uri = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.buf = buf->buf + (request_uri - buffer),
                            .len = request_uri_len},
      msg);

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_REQUEST_LINE);

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

  msg->status_line.sip_proto_ver = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.buf = buf->buf, .len = sip_version_len}, msg);

  msg->status_line.status_code = code;

  msg->status_line.reason_phrase = cmsc_s_msg_to_bstring(
      &(struct cmsc_String){.buf = buf->buf + (reason_phrase - buffer),
                            .len = reason_phrase_len},
      msg);

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_STATUS_LINE);

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

  const char *line_max = NULL;
  for (uint32_t i = 1; i < buf->len; i++) {
    if (buf->buf[i] == '\n' && buf->buf[i - 1] == '\r') {
      line_max = buf->buf + i - 1;
      break;
    }
  }
  if (!line_max) {
    err = cme_error(EINVAL, "No CLRF");
    goto error_out;
  }

  const char *sip_version = NULL;
  for (uint32_t i = 3; i < buf->len; i++) {
    if (buf->buf[i] == '/' && buf->buf[i - 1] == 'P' &&
        buf->buf[i - 2] == 'I' && buf->buf[i - 3] == 'S') {
      sip_version = buf->buf + i - 3;
      break;
    }
  }

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

static inline cme_error_t cmsc_parse_sip_body(struct cmsc_Buffer *buf,
                                              struct cmsc_SipMessage *msg) {
  if (!cmsc_sipmsg_is_field_present(msg,
                                    cmsc_SupportedSipHeaders_CONTENT_LENGTH) ||
      msg->content_length == 0) {
    return 0;
  }

  msg->body.buf_offset = buf->buf - msg->_buf.buf;
  msg->body.len = buf->len;

  if (msg->body.len > msg->content_length) {
    msg->body.len = msg->content_length;
  }

  return 0;
}

#endif

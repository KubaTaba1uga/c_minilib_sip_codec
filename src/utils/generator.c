#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/buffer.h"

cme_error_t cmsc_generate_sip(const struct cmsc_SipMessage *msg,
                              uint32_t *buf_len, const char **buf) {
  // TO-DO: validate msg content befor generation
  const uint32_t first_line_size = 255;
  char first_line[first_line_size];
  uint32_t first_line_len = 0;
  cme_error_t err;
  if (!msg || !buf_len || !buf) {
    err = cme_error(EINVAL, "`msg`, `buf_len` and `buf` cannot be NULL");
    goto error_out;
  }

  if (msg->request_line.sip_method.buf) {
    /*
      According RFC 3261 25 request line looks like this:
        Method SP Request-URI SP SIP-Version CRLF
    */
    first_line_len = snprintf(
        first_line, first_line_size, "%.*s %.*s %.*s\r\n",
        msg->request_line.sip_method.len, msg->request_line.sip_method.buf,
        msg->request_line.request_uri.len, msg->request_line.request_uri.buf,
        msg->request_line.sip_proto_ver.len,
        msg->request_line.sip_proto_ver.buf);
  } else {
    /*
      According RFC 3261 25 status line looks like this:
        SIP-Version SP Status-Code SP Reason-Phrase CRLF
    */
    first_line_len = snprintf(
        (char *)first_line, first_line_size, "%.*s %d %.*s\r\n",
        msg->status_line.sip_proto_ver.len, msg->status_line.sip_proto_ver.buf,
        msg->status_line.status_code, msg->status_line.reason_phrase.len,
        msg->status_line.reason_phrase.buf);
  }

  struct cmsc_Buffer local_buf = {
      .len = 0,
      .size = msg->_buf.len + first_line_len,
      .buf = calloc(msg->_buf.len + first_line_len, sizeof(char))};
  if (!local_buf.buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_buf`");
    goto error_out;
  }

  cmsc_buffer_insert(
      (struct cmsc_String){.buf = first_line, .len = first_line_len},
      &local_buf, NULL);

  struct cmsc_SipHeader *hdr;
  STAILQ_FOREACH(hdr, &msg->sip_headers, _next) {
    local_buf.len +=
        snprintf((char *)local_buf.buf + local_buf.len,
                 local_buf.size - local_buf.len, "%.*s: %.*s\r\n", hdr->key.len,
                 hdr->key.buf, hdr->value.len, hdr->value.buf);
  };

  *buf = local_buf.buf;
  *buf_len = local_buf.len;

  return 0;

error_out:
  return cme_return(err);
};

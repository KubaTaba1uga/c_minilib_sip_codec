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
  if (msg->request_line.sip_proto_ver.len) {
    /*
      According RFC 3261 25 request line looks like this:
        Method SP Request-URI SP SIP-Version CRLF
    */
    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s %.*s %.*s\r\n",
        msg->request_line.sip_method.len,
        cmsc_bs_msg_to_string(&msg->request_line.sip_method,
                              (struct cmsc_SipMessage *)msg)
            .buf,
        msg->request_line.request_uri.len,
        cmsc_bs_msg_to_string(&msg->request_line.request_uri,
                              (struct cmsc_SipMessage *)msg)
            .buf,
        msg->request_line.sip_proto_ver.len,
        cmsc_bs_msg_to_string(&msg->request_line.sip_proto_ver,
                              (struct cmsc_SipMessage *)msg)
            .buf);
    if (err) {
      goto error_local_buf_cleanup;
    }
  } else if (msg->status_line.sip_proto_ver.len) {
    /*
      According RFC 3261 25 status line looks like this:
        SIP-Version SP Status-Code SP Reason-Phrase CRLF
    */
    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s %d %.*s\r\n",
        msg->status_line.sip_proto_ver.len,
        cmsc_bs_msg_to_string(&msg->status_line.sip_proto_ver,
                              (struct cmsc_SipMessage *)msg)
            .buf,
        msg->status_line.status_code, msg->status_line.reason_phrase.len,
        cmsc_bs_msg_to_string(&msg->status_line.reason_phrase,
                              (struct cmsc_SipMessage *)msg)
            .buf);
    if (err) {
      goto error_local_buf_cleanup;
    }
  }

  struct cmsc_SipHeader *hdr;
  STAILQ_FOREACH(hdr, &msg->sip_headers, _next) {
    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s: %.*s\r\n", hdr->key.len,
        cmsc_bs_msg_to_string(&hdr->key, (struct cmsc_SipMessage *)msg).buf,
        hdr->value.len,
        cmsc_bs_msg_to_string(&hdr->value, (struct cmsc_SipMessage *)msg).buf);
    if (err) {
      goto error_local_buf_cleanup;
    }
  };

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

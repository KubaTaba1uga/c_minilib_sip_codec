#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"
#include "utils/buffer.h"
#include "utils/sipmsg.h"

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
      .size = msg->_buf.len,
      .buf = calloc(msg->_buf.len + 127, sizeof(char))};
  if (!local_buf.buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_buf`");
    goto error_out;
  }

  if (msg->request_line.sip_method.buf) {
    /*
      According RFC 3261 25 request line looks like this:
        Method SP Request-URI SP SIP-Version CRLF
    */
    // ADDED DBG: dump the three slices
    printf("[DBG] Generating request-line: method='%.*s' (%u), uri='%.*s' "
           "(%u), version='%.*s' (%u)\n",
           msg->request_line.sip_method.len, msg->request_line.sip_method.buf,
           msg->request_line.sip_method.len, msg->request_line.request_uri.len,
           msg->request_line.request_uri.buf, msg->request_line.request_uri.len,
           msg->request_line.sip_proto_ver.len,
           msg->request_line.sip_proto_ver.buf,
           msg->request_line.sip_proto_ver.len);

    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s %.*s %.*s\r\n",
        msg->request_line.sip_method.len, msg->request_line.sip_method.buf,
        msg->request_line.request_uri.len, msg->request_line.request_uri.buf,
        msg->request_line.sip_proto_ver.len,
        msg->request_line.sip_proto_ver.buf);
    if (err) {
      goto error_local_buf_cleanup;
    }
  } else {
    /*
      According RFC 3261 25 status line looks like this:
        SIP-Version SP Status-Code SP Reason-Phrase CRLF
    */
    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s %d %.*s\r\n",
        msg->status_line.sip_proto_ver.len, msg->status_line.sip_proto_ver.buf,
        msg->status_line.status_code, msg->status_line.reason_phrase.len,
        msg->status_line.reason_phrase.buf);
    if (err) {
      goto error_local_buf_cleanup;
    }
  }

  printf("Localbuf.len=%d, localbuf.size=%d\n", local_buf.len, local_buf.size);

  struct cmsc_SipHeader *hdr;
  STAILQ_FOREACH(hdr, &msg->sip_headers, _next) {
    err = cmsc_buffer_finsert(
        &local_buf, NULL, "%.*s: %.*s\r\n", hdr->key.len,
        cmsc_sipmsg_bstring_dump_string(&hdr->key, msg), hdr->value.len,
        cmsc_sipmsg_bstring_dump_string(&hdr->value, msg));
    if (err) {
      goto error_local_buf_cleanup;
    }
  };

  *buf = local_buf.buf;
  *buf_len = local_buf.len;

  return 0;
error_local_buf_cleanup:
  free((void *)local_buf.buf);
error_out:
  return cme_return(err);
};

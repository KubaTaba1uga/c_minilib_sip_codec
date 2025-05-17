#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#include "utils/buffer.h"
#include "utils/siphdr.h"
#include "utils/sipmsg.h"
#include <stdint.h>
#include <string.h>

// This function assumes ownership over _buf memory is transferred to msg.
cme_error_t cmsc_sipmsg_create_with_buf(struct cmsc_SipMessage **msg) {
  cme_error_t err;

  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  struct cmsc_Buffer buf;
  buf.buf = malloc(sizeof(char) * CMSC_SIPMSG_DEFAULT_BUF_SIZE);
  if (!buf.buf) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `buf.buf`");
    goto error_out;
  }

  buf.len = 0;
  buf.size = CMSC_SIPMSG_DEFAULT_BUF_SIZE;

  return cmsc_sipmsg_create(buf, msg);

error_out:
  return cme_return(err);
}

// This function assumes user ownership over _buf memory is transferred to msg
//  it also assumes memory was allocated dynamically with malloc.
void cmsc_sipmsg_destroy_with_buf(struct cmsc_SipMessage **msg) {
  if (!msg || !*msg) {
    return;
  }

  free((void *)(*msg)->_buf.buf);

  cmsc_sipmsg_destroy(msg);
}

cme_error_t
cmsc_sipmsg_insert_request_line(uint32_t sip_ver_len, const char *sip_ver,
                                uint32_t req_uri_len, const char *req_uri,
                                uint32_t sip_method_len, const char *sip_method,
                                struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!sip_ver || !req_uri || !sip_method || !msg) {
    err = cme_error(
        EINVAL, "`sip_ver`, `req_uri`, `sip_method` and `msg` cannot be NULL");
    goto error_out;
  }

  if (sip_ver_len == 0 || req_uri_len == 0 || sip_method_len == 0) {
    return 0;
  }

  // Insert Sip Version
  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = sip_ver, .len = sip_ver_len}, &msg->_buf,
      &msg->request_line.sip_proto_ver);
  if (err) {
    goto error_out;
  }

  // Insert Request Uri
  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = req_uri, .len = req_uri_len}, &msg->_buf,
      &msg->request_line.request_uri);
  if (err) {
    goto error_out;
  }

  // Insert Method
  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = sip_method, .len = sip_method_len},
      &msg->_buf, &msg->request_line.sip_method);
  if (err) {
    goto error_out;
  }

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_status_line(uint32_t sip_ver_len,
                                           const char *sip_ver,
                                           uint32_t reason_phrase_len,
                                           const char *reason_phrase,
                                           uint32_t status_code,
                                           struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!sip_ver || !reason_phrase || !status_code || !msg) {
    err = cme_error(
        EINVAL,
        "`sip_ver`, `reason_phrase`, `status_code` and `msg` cannot be NULL");
    goto error_out;
  }

  if (sip_ver_len == 0 || reason_phrase_len == 0) {
    return 0;
  }

  // Insert Sip Version
  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = sip_ver, .len = sip_ver_len}, &msg->_buf,
      &msg->request_line.sip_proto_ver);
  if (err) {
    goto error_out;
  }

  // Insert Reason Phrase
  err = cmsc_buffer_insert(
      (struct cmsc_String){.buf = reason_phrase, .len = reason_phrase_len},
      &msg->_buf, &msg->request_line.request_uri);
  if (err) {
    goto error_out;
  }

  msg->status_line.status_code = status_code;

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_header(uint32_t key_len, const char *key,
                                      uint32_t value_len, const char *value,
                                      struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!key || !msg) {
    err = cme_error(EINVAL, "`key` and `msg` cannot be NULL");
    goto error_out;
  }

  if (key_len == 0) {
    return 0;
  }

  struct cmsc_String msg_key = {0};
  err = cmsc_buffer_insert((struct cmsc_String){.buf = key, .len = key_len},
                           &msg->_buf, &msg_key);
  if (err) {
    goto error_out;
  }

  struct cmsc_String msg_value = {0};
  if (value) {
    err =
        cmsc_buffer_insert((struct cmsc_String){.buf = value, .len = value_len},
                           &msg->_buf, &msg_value);
    if (err) {
      goto error_out;
    }
  }

  struct cmsc_SipHeader *sip_hdr;
  err = cmsc_siphdr_create(msg_key.len, msg_key.buf, msg_value.len,
                           msg_value.buf, &sip_hdr);
  if (err) {
    goto error_out;
  }

  STAILQ_INSERT_TAIL(&msg->sip_headers, sip_hdr, _next);

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_body(const uint32_t body_len, const char *body,
                                    struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!body || !msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  if (body_len == 0) {
    return 0;
  }

  err = cmsc_buffer_insert((struct cmsc_String){.buf = body, .len = body_len},
                           &msg->_buf, &msg->body);
  if (err) {
    goto error_out;
  }

  msg->content_length = body_len;
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CONTENT_LENGTH);

  return 0;

error_out:
  return cme_return(err);
};

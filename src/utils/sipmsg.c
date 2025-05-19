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
  // Debug print
  printf("[insert_request_line] method='%.*s' (%u), uri='%.*s' (%u), "
         "version='%.*s' (%u)\n",
         sip_method_len, sip_method, sip_method_len, req_uri_len, req_uri,
         req_uri_len, sip_ver_len, sip_ver, sip_ver_len);
  if (sip_ver_len == 0 || req_uri_len == 0 || sip_method_len == 0) {
    return 0;
  }

  // Insert Sip Version
  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = sip_ver, .len = sip_ver_len}, &msg->_buf,
      &msg->request_line.sip_proto_ver);
  if (err) {
    goto error_out;
  }

  // Insert Request Uri
  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = req_uri, .len = req_uri_len}, &msg->_buf,
      &msg->request_line.request_uri);
  if (err) {
    goto error_out;
  }

  // Insert Method
  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = sip_method, .len = sip_method_len},
      &msg->_buf, &msg->request_line.sip_method);
  if (err) {
    goto error_out;
  }

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_REQUEST_LINE);

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
  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = sip_ver, .len = sip_ver_len}, &msg->_buf,
      &msg->status_line.sip_proto_ver);
  if (err) {
    goto error_out;
  }

  // Insert Reason Phrase
  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = reason_phrase, .len = reason_phrase_len},
      &msg->_buf, &msg->status_line.reason_phrase);
  if (err) {
    goto error_out;
  }

  msg->status_line.status_code = status_code;

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_STATUS_LINE);

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

  struct cmsc_BString msg_key = {0};
  err = cmsc_buffer_binsert((struct cmsc_String){.buf = key, .len = key_len},
                            &msg->_buf, &msg_key);
  if (err) {
    goto error_out;
  }

  struct cmsc_BString msg_value = {0};
  if (value) {
    err = cmsc_buffer_binsert(
        (struct cmsc_String){.buf = value, .len = value_len}, &msg->_buf,
        &msg_value);
    if (err) {
      goto error_out;
    }
  }

  struct cmsc_SipHeader *sip_hdr;
  err = cmsc_siphdr_create(msg_key.len, msg_key.buf_offset, msg_value.len,
                           msg_value.buf_offset, &sip_hdr);
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

  err = cmsc_buffer_binsert((struct cmsc_String){.buf = body, .len = body_len},
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

cme_error_t cmsc_sipmsg_insert_to(uint32_t uri_len, const char *uri,
                                  uint32_t tag_len, const char *tag,
                                  struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!uri || !msg) {
    err = cme_error(EINVAL, "`uri` and `msg` cannot be NULL");
    goto error_out;
  }

  if (uri_len == 0) {
    return 0;
  }

  err = cmsc_buffer_binsert((struct cmsc_String){.buf = uri, .len = uri_len},
                            &msg->_buf, &msg->to.uri);
  if (err) {
    goto error_out;
  }

  if (tag && tag_len > 0) {
    err = cmsc_buffer_binsert((struct cmsc_String){.buf = tag, .len = tag_len},
                              &msg->_buf, &msg->to.tag);
    if (err) {
      goto error_out;
    }
  }

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_TO);

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_from(uint32_t uri_len, const char *uri,
                                    uint32_t tag_len, const char *tag,
                                    struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!uri || !msg) {
    err = cme_error(EINVAL, "`uri` and `msg` cannot be NULL");
    goto error_out;
  }

  if (uri_len == 0) {
    return 0;
  }

  err = cmsc_buffer_binsert((struct cmsc_String){.buf = uri, .len = uri_len},
                            &msg->_buf, &msg->from.uri);
  if (err) {
    goto error_out;
  }

  if (tag && tag_len > 0) {
    err = cmsc_buffer_binsert((struct cmsc_String){.buf = tag, .len = tag_len},
                              &msg->_buf, &msg->from.tag);
    if (err) {
      goto error_out;
    }
  }

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_FROM);

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_call_id(uint32_t call_id_len,
                                       const char *call_id,
                                       struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!call_id || !msg) {
    err = cme_error(EINVAL, "`call_id` and `msg` cannot be NULL");
    goto error_out;
  }

  if (call_id_len == 0) {
    return 0;
  }

  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = call_id, .len = call_id_len}, &msg->_buf,
      &msg->call_id);
  if (err) {
    goto error_out;
  }

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CALL_ID);

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_sipmsg_insert_cseq(uint32_t sip_method_len,
                                    const char *sip_method, uint32_t seq_number,
                                    struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!sip_method || !seq_number || !msg) {
    err = cme_error(EINVAL,
                    "`sip_method`, `seq_number` and `msg` cannot be NULL");
    goto error_out;
  }

  if (sip_method_len == 0) {
    return 0;
  }

  err = cmsc_buffer_binsert(
      (struct cmsc_String){.buf = sip_method, .len = sip_method_len},
      &msg->_buf, &msg->cseq.method);
  if (err) {
    goto error_out;
  }

  msg->cseq.seq_number = seq_number;

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CSEQ);

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t cmsc_sipmsg_insert_max_forwards(uint32_t max_forwards,
                                            struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  msg->max_forwards = max_forwards;

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_MAX_FORWARDS);

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t cmsc_sipmsg_insert_content_length(uint32_t content_length,
                                              struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!msg) {
    err = cme_error(EINVAL, "`msg` cannot be NULL");
    goto error_out;
  }

  msg->content_length = content_length;

  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_CONTENT_LENGTH);

  return 0;

error_out:
  return cme_return(err);
}

cme_error_t cmsc_sipmsg_insert_via(uint32_t proto_len, const char *proto,
                                   uint32_t sent_by_len, const char *sent_by,
                                   uint32_t addr_len, const char *addr,
                                   uint32_t branch_len, const char *branch,
                                   uint32_t received_len, const char *received,
                                   uint32_t ttl, struct cmsc_SipMessage *msg) {
  cme_error_t err;

  if (!msg || !proto || !sent_by) {
    err = cme_error(EINVAL, "`msg`, `proto` and `sent_by` cannot be NULL");
    goto error_out;
  }

  if (!proto_len || !sent_by_len) {
    return 0;
  }

  struct cmsc_SipHeaderVia *via;
  via = calloc(1, sizeof(struct cmsc_SipHeaderVia));
  if (!via) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `via`");
    goto error_via_cleanup;
  }

  err =
      cmsc_buffer_binsert((struct cmsc_String){.len = proto_len, .buf = proto},
                          &msg->_buf, &via->proto);
  if (err) {
    goto error_via_cleanup;
  }

  err = cmsc_buffer_binsert(
      (struct cmsc_String){.len = sent_by_len, .buf = sent_by}, &msg->_buf,
      &via->sent_by);
  if (err) {
    goto error_via_cleanup;
  }

  if (addr_len && addr) {
    err =
        cmsc_buffer_binsert((struct cmsc_String){.len = addr_len, .buf = addr},
                            &msg->_buf, &via->addr);
    if (err) {
      goto error_via_cleanup;
    }
  }

  if (branch_len && branch) {
    err = cmsc_buffer_binsert(
        (struct cmsc_String){.len = branch_len, .buf = branch}, &msg->_buf,
        &via->branch);
    if (err) {
      goto error_via_cleanup;
    }
  }

  if (received_len && received) {
    err = cmsc_buffer_binsert(
        (struct cmsc_String){.len = received_len, .buf = received}, &msg->_buf,
        &via->received);
    if (err) {
      goto error_via_cleanup;
    }
  }

  via->ttl = ttl;

  STAILQ_INSERT_TAIL(&msg->vias, via, _next);
  cmsc_sipmsg_mark_field_present(msg, cmsc_SupportedSipHeaders_VIAS);

  return 0;

error_via_cleanup:
  free(via);
error_out:
  return cme_return(err);
}

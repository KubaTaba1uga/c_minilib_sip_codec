#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#define INPUT_FILE "message.sip"

int main(void) {
  cme_error_t err = cmsc_init();
  if (err)
    goto error_out;

  // Load file
  FILE *f = fopen(INPUT_FILE, "rb");
  if (!f) {
    perror("fopen");
    goto error_out;
  }
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  rewind(f);

  char *buf = malloc(len + 1);
  fread(buf, 1, len, f);
  fclose(f);
  buf[len] = 0;

  struct cmsc_SipMessage *msg = NULL;
  err = cmsc_parse_sip((uint32_t)len, buf, &msg);
  free(buf);
  if (err)
    goto error_out;

  printf("=== Parsed SIP ===\n");

  struct cmsc_String method =
      cmsc_bs_msg_to_string(&msg->request_line.sip_method, msg);
  struct cmsc_String uri =
      cmsc_bs_msg_to_string(&msg->request_line.request_uri, msg);
  struct cmsc_String version =
      cmsc_bs_msg_to_string(&msg->request_line.sip_proto_ver, msg);
  printf("Request Line: %.*s %.*s %.*s\n", method.len, method.buf, uri.len,
         uri.buf, version.len, version.buf);

  struct cmsc_String from = cmsc_bs_msg_to_string(&msg->from.uri, msg);
  struct cmsc_String from_tag = cmsc_bs_msg_to_string(&msg->from.tag, msg);
  printf("From: %.*s (tag=%.*s)\n", from.len, from.buf, from_tag.len,
         from_tag.buf);

  struct cmsc_String to = cmsc_bs_msg_to_string(&msg->to.uri, msg);
  struct cmsc_String to_tag = cmsc_bs_msg_to_string(&msg->to.tag, msg);
  printf("To: %.*s (tag=%.*s)\n", to.len, to.buf, to_tag.len, to_tag.buf);

  struct cmsc_String call_id = cmsc_bs_msg_to_string(&msg->call_id, msg);
  printf("Call-ID: %.*s\n", call_id.len, call_id.buf);

  struct cmsc_String method_cseq =
      cmsc_bs_msg_to_string(&msg->cseq.method, msg);
  printf("CSeq: %u %.*s\n", msg->cseq.seq_number, method_cseq.len,
         method_cseq.buf);

  struct cmsc_SipHeaderVia *via = STAILQ_FIRST(&msg->vias);
  if (via) {
    struct cmsc_String proto = cmsc_bs_msg_to_string(&via->proto, msg);
    struct cmsc_String sent_by = cmsc_bs_msg_to_string(&via->sent_by, msg);
    struct cmsc_String branch = cmsc_bs_msg_to_string(&via->branch, msg);
    printf("Via: %.*s %.*s; branch=%.*s\n", proto.len, proto.buf, sent_by.len,
           sent_by.buf, branch.len, branch.buf);
  }

  // Custom headers
  struct cmsc_SipHeader *h;
  STAILQ_FOREACH(h, &msg->sip_headers, _next) {
    struct cmsc_String key = cmsc_bs_msg_to_string(&h->key, msg);
    struct cmsc_String val = cmsc_bs_msg_to_string(&h->value, msg);
    printf("Header: %.*s: %.*s\n", key.len, key.buf, val.len, val.buf);
  }

  // Body
  struct cmsc_String body = cmsc_bs_msg_to_string(&msg->body, msg);
  printf("Body (%u bytes): %.*s\n", body.len, body.len, body.buf);

  cmsc_sipmsg_destroy(&msg);
  cmsc_destroy();
  return 0;

error_out:
  fprintf(stderr, "Parsing failed\n");
  if (err) {
    cme_error_dump_to_file(err, "parse_error.txt");
    cme_error_destroy(err);
  }
  cmsc_destroy();
  return 1;
}

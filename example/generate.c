#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "c_minilib_sip_codec.h"

#define OUTPUT_FILE "message.sip"

int main(void) {
  cme_error_t err = cmsc_init();
  if (err)
    goto error_out;

  struct cmsc_SipMessage *msg = NULL;
  err = cmsc_sipmsg_create_with_buf(&msg);
  if (err)
    goto error_out;

  // Basic SIP fields
  err = cmsc_sipmsg_insert_request_line(
      strlen("SIP/2.0"), "SIP/2.0", strlen("sip:bob@example.com"),
      "sip:bob@example.com", strlen("INVITE"), "INVITE", msg);
  if (err)
    goto error_out;

  err = cmsc_sipmsg_insert_from(strlen("<sip:alice@example.com>"),
                                "<sip:alice@example.com>", strlen("a1b2c3"),
                                "a1b2c3", msg);
  if (err)
    goto error_out;

  err = cmsc_sipmsg_insert_to(strlen("<sip:bob@example.com>"),
                              "<sip:bob@example.com>", strlen("x9y8z7"),
                              "x9y8z7", msg);
  if (err)
    goto error_out;

  err = cmsc_sipmsg_insert_call_id(strlen("call-1234"), "call-1234", msg);
  if (err)
    goto error_out;

  err = cmsc_sipmsg_insert_cseq(strlen("INVITE"), "INVITE", 42, msg);
  if (err)
    goto error_out;

  err = cmsc_sipmsg_insert_via(
      strlen("SIP/2.0/UDP"), "SIP/2.0/UDP", strlen("client.example.com"),
      "client.example.com", 0, NULL, strlen("z9hG4bKbranch123"),
      "z9hG4bKbranch123", 0, NULL, 0, msg);
  if (err)
    goto error_out;

  // Optional extra header
  err = cmsc_sipmsg_insert_header(strlen("X-Debug"), "X-Debug", strlen("on"),
                                  "on", msg);
  if (err)
    goto error_out;

  // Add body
  const char *body = "Hello from the SIP body!";
  err = cmsc_sipmsg_insert_body(strlen(body), body, msg);
  if (err)
    goto error_out;

  // Generate raw SIP message
  const char *out_buf = NULL;
  uint32_t out_len = 0;
  err = cmsc_generate_sip(msg, &out_len, &out_buf);
  if (err)
    goto error_out;

  FILE *f = fopen(OUTPUT_FILE, "w");
  if (!f) {
    perror("fopen");
    free((void *)out_buf);
    goto error_out;
  }

  fwrite(out_buf, 1, out_len, f);
  fclose(f);
  printf("SIP message written to %s (%u bytes)\n", OUTPUT_FILE, out_len);

  free((void *)out_buf);
  cmsc_sipmsg_destroy_with_buf(&msg);
  cmsc_destroy();
  return 0;

error_out:
  fprintf(stderr, "Error during generation\n");
  if (err) {
    cme_error_dump_to_file(err, "generate_error.txt");
    cme_error_destroy(err);
  }
  cmsc_sipmsg_destroy_with_buf(&msg);
  cmsc_destroy();
  return 1;
}

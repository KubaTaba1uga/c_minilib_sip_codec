#include <c_minilib_error.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "utils/sipmsg.h"

inline static void make_msg(const char *raw, struct cmsc_SipMessage **msg) {
  struct cmsc_Buffer buf = {
      .buf = raw, .len = (uint32_t)strlen(raw), .size = (uint32_t)strlen(raw)};

  cme_error_t err = cmsc_sipmsg_create(buf, msg);
  TEST_ASSERT_NULL(err);
}

inline static void create_msg(const char *raw, struct cmsc_SipMessage **msg) {

  char *raw_buf = malloc(strlen(raw) + 1);
  TEST_ASSERT_NOT_NULL(raw_buf);
  memcpy((void *)raw_buf, raw, strlen(raw));
  raw_buf[strlen(raw)] = 0;

  struct cmsc_Buffer buf = {.buf = raw_buf,
                            .len = (uint32_t)strlen(raw),
                            .size = (uint32_t)strlen(raw)};
  puts(raw);
  cme_error_t err = cmsc_sipmsg_create(buf, msg);
  TEST_ASSERT_NULL(err);
}
inline static struct cmsc_SipHeader *create_hdr(struct cmsc_SipMessage *msg) {
  struct cmsc_SipHeader *hdr = calloc(1, sizeof(struct cmsc_SipHeader));
  TEST_ASSERT_NOT_NULL(hdr);

  const char *buf_start = msg->_buf.buf;
  const char *colon = strchr(buf_start, ':');
  TEST_ASSERT_NOT_NULL(colon);

  // Key is from start up to the colon
  hdr->key.buf_offset = 0;
  hdr->key.len = (uint32_t)(colon - buf_start);

  // Skip colon and optional space
  const char *value_start = colon + 1;
  if (*value_start == ' ')
    value_start++;

  hdr->value.buf_offset = (uint32_t)(value_start - buf_start);
  hdr->value.len = (uint32_t)(msg->_buf.len - hdr->value.buf_offset);

  STAILQ_INSERT_TAIL(&msg->sip_headers, hdr, _next);

  return hdr;
}

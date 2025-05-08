#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include <c_minilib_error.h>

#include "utils/dynamic_buffer.h"
#define INITIAL_SIZE 8
#define LARGE_DATA "0123456789ABCDEF0123456789ABCDEF"

static struct cmsc_DynamicBuffer *buf = NULL;

void setUp(void) { cme_init(); }

void tearDown(void) {
  cmsc_dynbuf_destroy(&buf);
  cme_destroy();
}

void test_create_valid(void) {
  cme_error_t err = cmsc_dynbuf_create(INITIAL_SIZE, &buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(buf);
  TEST_ASSERT_EQUAL_UINT32(0, buf->len);
  TEST_ASSERT_EQUAL_UINT32(INITIAL_SIZE, buf->size);
}

void test_create_null_output(void) {
  cme_error_t err = cmsc_dynbuf_create(INITIAL_SIZE, NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_destroy_null_pointer(void) {
  // Should be a no-op, not crash
  cmsc_dynbuf_destroy(NULL);
}

void test_destroy_already_destroyed(void) {
  cmsc_dynbuf_create(INITIAL_SIZE, &buf);
  cmsc_dynbuf_destroy(&buf);
  TEST_ASSERT_NULL(buf);
  // Second destroy should also be safe
  cmsc_dynbuf_destroy(&buf);
  TEST_ASSERT_NULL(buf);
}

void test_put_within_capacity(void) {
  cmsc_dynbuf_create(INITIAL_SIZE, &buf);
  const char *data = "ABCD";
  cme_error_t err = cmsc_dynbuf_put(strlen(data), (char *)data, &buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL_MEMORY(data, buf->buf, strlen(data));
  TEST_ASSERT_EQUAL_UINT32(4, buf->len);
}

void test_put_triggers_realloc(void) {
  cmsc_dynbuf_create(4, &buf);
  size_t data_len = strlen(LARGE_DATA);
  cme_error_t err =
      cmsc_dynbuf_put((uint32_t)data_len, (char *)LARGE_DATA, &buf);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_TRUE(buf->size >= data_len);
  TEST_ASSERT_EQUAL_MEMORY(LARGE_DATA, buf->buf, data_len);
}

void test_put_null_dynbuf_ptr(void) {
  cme_error_t err = cmsc_dynbuf_put(4, "test", NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_put_null_buffer_data_zero_len(void) {
  cmsc_dynbuf_create(INITIAL_SIZE, &buf);
  // data == NULL but length == 0 ⇒ memcpy of 0 bytes is valid
  cme_error_t err = cmsc_dynbuf_put(0, NULL, &buf);
  TEST_ASSERT_NULL(err);
}

void test_flush_reduces_size_and_resets_len(void) {
  cme_error_t err = cmsc_dynbuf_create(INITIAL_SIZE * 4, &buf);
  TEST_ASSERT_NULL(err);
  // simulate that len was less than half the size
  buf->len = INITIAL_SIZE;
  char *p = cmsc_dynbuf_flush(buf);
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_UINT32(0, buf->len);
  TEST_ASSERT_EQUAL_UINT32((INITIAL_SIZE * 4) / 2, buf->size);
}

void test_flush_null_buffer(void) {
  char *out = cmsc_dynbuf_flush(NULL);
  TEST_ASSERT_NULL(out);
}

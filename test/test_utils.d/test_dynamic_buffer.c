#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "utils/dynamic_buffer.h"
#include <c_minilib_error.h>

#define INITIAL_SIZE 8
#define LARGE_DATA "0123456789ABCDEF0123456789ABCDEF"

struct TestContainer {
  int dummy; // Example header data before dynbuf
  struct cmsc_DynamicBuffer dynbuf;
};

static void *container = NULL;
static struct TestContainer *test_container = NULL;
static struct cmsc_DynamicBuffer *buf = NULL;
static size_t container_size = 0;

void setUp(void) {
  cme_init();
  container_size = sizeof(struct TestContainer) + INITIAL_SIZE;
  container = malloc(container_size);
  TEST_ASSERT_NOT_NULL(container);

  test_container = (struct TestContainer *)container;
  buf = &test_container->dynbuf;

  cme_error_t err = cmsc_dynbuf_init(INITIAL_SIZE, buf);
  TEST_ASSERT_NULL(err);
}

void tearDown(void) {
  free(container);
  container = NULL;
  test_container = NULL;
  buf = NULL;
  container_size = 0;
  cme_destroy();
}

void test_init_null(void) {
  cme_error_t err = cmsc_dynbuf_init(INITIAL_SIZE, NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_put_within_capacity(void) {
  const char *data = "ABCD";
  cme_error_t err = cmsc_dynbuf_put(strlen(data), (char *)data, &container,
                                    container_size, buf);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_EQUAL_MEMORY(data, buf->buf, strlen(data));
  TEST_ASSERT_EQUAL_UINT32(4, buf->len);
}

void test_put_triggers_realloc(void) {
  const char *data = LARGE_DATA;
  size_t data_len = strlen(data);

  cme_error_t err =
      cmsc_dynbuf_put(data_len, (char *)data, &container, container_size, buf);
  TEST_ASSERT_NULL(err);

  test_container = (struct TestContainer *)container;
  buf = &test_container->dynbuf;

  TEST_ASSERT_TRUE(buf->size >= data_len);
  TEST_ASSERT_EQUAL_MEMORY(data, buf->buf, data_len);
  TEST_ASSERT_EQUAL_UINT32(data_len, buf->len);
}

void test_put_null_dynbuf_ptr(void) {
  const char *data = "test";
  cme_error_t err = cmsc_dynbuf_put(strlen(data), (char *)data, &container,
                                    container_size, NULL);
  TEST_ASSERT_NOT_NULL(err);
}

void test_put_null_data_zero_len(void) {
  cme_error_t err = cmsc_dynbuf_put(0, NULL, &container, container_size, buf);
  TEST_ASSERT_NULL(err);
}

void test_flush_reduces_size_and_resets_len(void) {
  // simulate buffer usage
  cmsc_dynbuf_put(3, "ABC", &container, container_size, buf);
  char *p = cmsc_dynbuf_flush(buf);

  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_UINT32(0, buf->len);
  TEST_ASSERT_EQUAL_UINT32(INITIAL_SIZE / 2, buf->size);
}

void test_flush_null_buffer(void) {
  char *out = cmsc_dynbuf_flush(NULL);
  TEST_ASSERT_NULL(out);
}

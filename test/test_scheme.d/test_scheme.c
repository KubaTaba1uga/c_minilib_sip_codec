#include <stdlib.h>
#include <string.h>

#include <c_minilib_error.h>
#include <unity.h>

#include "scheme/cmsc_scheme.h"

static int dummy_generate(void *unused1, char *unused2, size_t *unused3) {
  return 0;
}

static int dummy_parse(void *unused1, const char *unused2, size_t unused3) {
  return 0;
}

void test_cmsc_scheme_create_and_destroy() {
  struct cmsc_Scheme *scheme = NULL;
  cme_error_t err = cmsc_scheme_create(cmsc_SipMsgType_INVITE, &scheme);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(scheme);
  TEST_ASSERT_EQUAL(scheme->sip_msg_type, cmsc_SipMsgType_INVITE);
  TEST_ASSERT_EQUAL(scheme->mandatory_fields_len, 0);
  TEST_ASSERT_EQUAL(scheme->optional_fields_len, 0);
  cmsc_scheme_destroy(&scheme);
  TEST_ASSERT_NULL(scheme);
}

void test_cmsc_scheme_add_mandatory_field_valid() {
  struct cmsc_Scheme *scheme = NULL;
  struct cmsc_SchemeField field = {
      .id = "Via",
      .generate_field_func = dummy_generate,
      .parse_field_func = dummy_parse,
  };

  cme_error_t err = cmsc_scheme_create(cmsc_SipMsgType_INVITE, &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(&field, scheme);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(scheme->mandatory_fields_len, 1);
  TEST_ASSERT_NOT_NULL(scheme->mandatory_fields);
  TEST_ASSERT(strcmp(scheme->mandatory_fields[0].id, "Via") == 0);
  cmsc_scheme_destroy(&scheme);
}

void test_cmsc_scheme_add_optional_field_valid() {
  struct cmsc_Scheme *scheme = NULL;
  struct cmsc_SchemeField field = {
      .id = "User-Agent",
      .generate_field_func = dummy_generate,
      .parse_field_func = dummy_parse,
  };

  cme_error_t err = cmsc_scheme_create(cmsc_SipMsgType_INVITE, &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_optional_field(&field, scheme);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(scheme->optional_fields_len, 1);
  TEST_ASSERT_NOT_NULL(scheme->optional_fields);
  TEST_ASSERT(strcmp(scheme->optional_fields[0].id, "User-Agent") == 0);
  cmsc_scheme_destroy(&scheme);
}

void test_cmsc_scheme_add_field_null_checks() {
  struct cmsc_Scheme *scheme = NULL;
  struct cmsc_SchemeField bad_field = {
      .id = NULL,
      .generate_field_func = dummy_generate,
      .parse_field_func = dummy_parse,
  };

  cme_error_t err = cmsc_scheme_create(cmsc_SipMsgType_INVITE, &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(NULL, scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(&bad_field, scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_optional_field(NULL, scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_optional_field(&bad_field, scheme);
  TEST_ASSERT_NOT_NULL(err);

  cmsc_scheme_destroy(&scheme);
}

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <c_minilib_error.h>
#include <unity.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"

static int dummy_generate(void *unused1, char *unused2, size_t *unused3) {
  return 0;
}

static cme_error_t dummy_parse(const uint32_t unused1, const char *unused2,
                               cmsc_sipmsg_t unused3) {
  return 0;
}

void test_cmsc_scheme_create_and_destroy(void) {
  struct cmsc_Scheme *scheme = NULL;
  cme_error_t err =
      cmsc_scheme_create(cmsc_SipMsgType_INVITE, "INVITE", &scheme);
  TEST_ASSERT_NULL(err);
  TEST_ASSERT_NOT_NULL(scheme);
  TEST_ASSERT_EQUAL(scheme->sip_msg_type, cmsc_SipMsgType_INVITE);
  TEST_ASSERT_EQUAL(scheme->mandatory.len, 0);
  TEST_ASSERT_EQUAL(scheme->optional.len, 0);
  cmsc_scheme_destroy(&scheme);
  TEST_ASSERT_NULL(scheme);
}

void test_cmsc_scheme_add_mandatory_field_valid(void) {
  struct cmsc_Scheme *scheme = NULL;
  cme_error_t err =
      cmsc_scheme_create(cmsc_SipMsgType_INVITE, "INVITE", &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(
      &(struct cmsc_SchemeField){
          .id = "Via",
          .generate_field_func = dummy_generate,
          .parse_field_func = dummy_parse,
      },
      scheme);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(scheme->mandatory.len, 1);
  TEST_ASSERT_NOT_NULL(scheme->mandatory.fields);
  TEST_ASSERT(strcmp(scheme->mandatory.fields[0].id, "Via") == 0);
  cmsc_scheme_destroy(&scheme);
}

void test_cmsc_scheme_add_optional_field_valid(void) {
  struct cmsc_Scheme *scheme = NULL;
  cme_error_t err =
      cmsc_scheme_create(cmsc_SipMsgType_INVITE, "INVITE", &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_optional_field(
      &(struct cmsc_SchemeField){
          .id = "User-Agent",
          .generate_field_func = dummy_generate,
          .parse_field_func = dummy_parse,
      },
      scheme);
  TEST_ASSERT_NULL(err);

  TEST_ASSERT_EQUAL(scheme->optional.len, 1);
  TEST_ASSERT_NOT_NULL(scheme->optional.fields);
  TEST_ASSERT(strcmp(scheme->optional.fields[0].id, "User-Agent") == 0);
  cmsc_scheme_destroy(&scheme);
}

void test_cmsc_scheme_add_field_null_checks(void) {
  struct cmsc_Scheme *scheme = NULL;
  cme_error_t err =
      cmsc_scheme_create(cmsc_SipMsgType_INVITE, "INVITE", &scheme);
  TEST_ASSERT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(NULL, scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_mandatory_field(
      &(struct cmsc_SchemeField){
          .id = NULL,
          .generate_field_func = dummy_generate,
          .parse_field_func = dummy_parse,
      },
      scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_optional_field(NULL, scheme);
  TEST_ASSERT_NOT_NULL(err);

  err = cmsc_scheme_add_optional_field(
      &(struct cmsc_SchemeField){
          .id = NULL,
          .generate_field_func = dummy_generate,
          .parse_field_func = dummy_parse,
      },
      scheme);
  TEST_ASSERT_NOT_NULL(err);

  cmsc_scheme_destroy(&scheme);
}

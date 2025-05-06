#include <c_minilib_error.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"

cme_error_t cmsc_scheme_create(const enum cmsc_SipMsgType msg_type,
                               const char *msg_type_str,
                               struct cmsc_Scheme **scheme) {
  struct cmsc_Scheme *local_scheme;
  cme_error_t err;

  local_scheme = malloc(sizeof(struct cmsc_Scheme));
  if (!local_scheme) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `local_scheme`");
    goto error_out;
  }

  local_scheme->sip_msg_type = msg_type;
  local_scheme->sip_msg_type_str = msg_type_str;

  local_scheme->mandatory_fields = NULL;
  local_scheme->mandatory_fields_len = 0;
  local_scheme->optional_fields = NULL;
  local_scheme->optional_fields_len = 0;

  *scheme = local_scheme;

  return 0;

error_out:
  return cme_return(err);
};

void cmsc_scheme_destroy(struct cmsc_Scheme **scheme) {
  if (!scheme || !*scheme) {
    return;
  }

  free((*scheme)->mandatory_fields);
  free((*scheme)->optional_fields);
  free((*scheme));
  *scheme = NULL;
};

cme_error_t
cmsc_scheme_add_mandatory_field(const struct cmsc_SchemeField *field,
                                struct cmsc_Scheme *scheme) {
  struct cmsc_SchemeField *mlocal_fields;
  cme_error_t err;

  if (!field || !scheme || !field->generate_field_func ||
      !field->parse_field_func || !field->id) {
    err = cme_error(
        EINVAL,
        "`field`, `field->generate_field_func`, "
        "`field->parse_field_func`, `field->id` and `scheme` cannot be NULL");
    goto error_out;
  }

  mlocal_fields =
      realloc(scheme->mandatory_fields, sizeof(struct cmsc_SchemeField) *
                                            (scheme->mandatory_fields_len + 1));
  if (!mlocal_fields) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `mlocal_fields`");
    goto error_out;
  }

  mlocal_fields[scheme->mandatory_fields_len++] = *field;
  scheme->mandatory_fields = mlocal_fields;

  return 0;

error_out:
  return cme_return(err);
};

cme_error_t cmsc_scheme_add_optional_field(const struct cmsc_SchemeField *field,
                                           struct cmsc_Scheme *scheme) {
  struct cmsc_SchemeField *olocal_fields;
  cme_error_t err;

  if (!field || !scheme || !field->generate_field_func ||
      !field->parse_field_func || !field->id) {
    err = cme_error(
        EINVAL,
        "`field`, `field->generate_field_func`, "
        "`field->parse_field_func`, `field->id` and `scheme` cannot be NULL");
    goto error_out;
  }

  olocal_fields =
      realloc(scheme->optional_fields, sizeof(struct cmsc_SchemeField) *
                                           (scheme->optional_fields_len + 1));
  if (!olocal_fields) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `olocal_fields`");
    goto error_out;
  }

  olocal_fields[scheme->optional_fields_len++] = *field;
  scheme->optional_fields = olocal_fields;

  return 0;

error_out:
  return cme_return(err);
};

#include <c_minilib_error.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"
#include "sipmsg/sipmsg.h"

cme_error_t cmsc_scheme_init(const enum cmsc_SupportedMessages supmsg,
                             struct cmsc_Scheme *scheme) {
  cme_error_t err;

  if (!scheme) {
    err = cme_error(EINVAL, "`scheme` cannot be NULL");
    goto error_out;
  }

  scheme->supmsg = supmsg;

  scheme->mandatory.fields = NULL;
  scheme->mandatory.size = 0;
  scheme->mandatory.len = 0;

  scheme->optional.fields = NULL;
  scheme->optional.size = 0;
  scheme->optional.len = 0;

  return 0;

error_out:
  return cme_return(err);
};

void cmsc_scheme_destroy(struct cmsc_Scheme *scheme) {
  if (!scheme) {
    return;
  }

  free(scheme->mandatory.fields);
  scheme->mandatory.size = 0;
  scheme->mandatory.len = 0;

  free(scheme->optional.fields);
  scheme->optional.size = 0;
  scheme->optional.len = 0;
};

cme_error_t
cmsc_scheme_add_mandatory_field(const struct cmsc_SchemeField *field,
                                struct cmsc_Scheme *scheme) {
  struct cmsc_SchemeField *mlocal_fields;
  cme_error_t err;

  if (!field || !scheme || !field->parse_field_func || !field->id) {
    err = cme_error(
        EINVAL,
        "`field`, `field->generate_field_func`, "
        "`field->parse_field_func`, `field->id` and `scheme` cannot be NULL");
    goto error_out;
  }

  mlocal_fields =
      realloc(scheme->mandatory.fields,
              sizeof(struct cmsc_SchemeField) * (scheme->mandatory.len + 1));
  if (!mlocal_fields) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `mlocal_fields`");
    goto error_out;
  }

  mlocal_fields[scheme->mandatory.len++] = *field;
  scheme->mandatory.fields = mlocal_fields;

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
      realloc(scheme->optional.fields,
              sizeof(struct cmsc_SchemeField) * (scheme->optional.len + 1));
  if (!olocal_fields) {
    err = cme_error(ENOMEM, "Cannot allocate memory for `olocal_fields`");
    goto error_out;
  }

  olocal_fields[scheme->optional.len++] = *field;
  scheme->optional.fields = olocal_fields;

  return 0;

error_out:
  return cme_return(err);
};

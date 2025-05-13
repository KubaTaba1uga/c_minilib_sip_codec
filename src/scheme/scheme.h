/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_SCHEME_H
#define C_MINILIB_SIP_CODEC_SCHEME_H

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"
#include <c_minilib_error.h>

// Iteration macros for scheme field lists
#define CMSC_FOREACH_SCHEME_MANDATORY(scheme_ptr, index_var, field_ptr)        \
  for ((index_var) = 0, (field_ptr) = &(scheme_ptr)->mandatory.fields[0];      \
       (index_var) < (scheme_ptr)->mandatory.len;                              \
       ++(index_var), ++(field_ptr))

#define CMSC_FOREACH_SCHEME_OPTIONAL(scheme_ptr, index_var, field_ptr)         \
  for ((index_var) = 0, (field_ptr) = &(scheme_ptr)->optional.fields[0];       \
       (index_var) < (scheme_ptr)->optional.len; ++(index_var), ++(field_ptr))

// Structure representing a field that belongs to a SIP message scheme
struct cmsc_SchemeField {
  const char *id;
  bool (*is_field_func)(const uint32_t buffer_len, const char *buffer);
  cme_error_t (*parse_field_func)(const struct cmsc_ValueLine *line,
                                  cmsc_sipmsg_t *msg);
  void *generate_field_func;
};

// List of fields (either mandatory or optional)
struct cmsc_SchemeFields {
  struct cmsc_SchemeField *fields;
  uint32_t size;
  uint32_t len;
};

// Complete scheme for a SIP message (INVITE, 200 OK, etc.)
struct cmsc_Scheme {
  enum cmsc_SupportedMessages supmsg;
  struct cmsc_SchemeFields mandatory;
  struct cmsc_SchemeFields optional;
};

// Initialize a scheme for a given supported message
static inline cme_error_t
cmsc_scheme_init(const enum cmsc_SupportedMessages supmsg,
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
}

// Clean up and deallocate a scheme
static inline void cmsc_scheme_destroy(struct cmsc_Scheme *scheme) {
  if (!scheme) {
    return;
  }

  free(scheme->mandatory.fields);
  scheme->mandatory.fields = NULL;
  scheme->mandatory.size = 0;
  scheme->mandatory.len = 0;

  free(scheme->optional.fields);
  scheme->optional.fields = NULL;
  scheme->optional.size = 0;
  scheme->optional.len = 0;
}

// Add a field to the mandatory section of a scheme
static inline cme_error_t
cmsc_scheme_add_mandatory_field(const struct cmsc_SchemeField *field,
                                struct cmsc_Scheme *scheme) {
  struct cmsc_SchemeField *mlocal_fields;
  cme_error_t err;

  if (!field || !scheme || !field->parse_field_func || !field->id) {
    err = cme_error(EINVAL, "`field`, `field->parse_field_func`, `field->id` "
                            "and `scheme` cannot be NULL");
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
}

// Add a field to the optional section of a scheme
static inline cme_error_t
cmsc_scheme_add_optional_field(const struct cmsc_SchemeField *field,
                               struct cmsc_Scheme *scheme) {
  struct cmsc_SchemeField *olocal_fields;
  cme_error_t err;

  if (!field || !scheme || !field->parse_field_func || !field->id) {
    err = cme_error(
        EINVAL,
        "`field`, "
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
}

#endif // C_MINILIB_SIP_CODEC_SCHEME_H

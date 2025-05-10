#ifndef C_MINILIB_SIP_CODEC_SCHEME_H
#define C_MINILIB_SIP_CODEC_SCHEME_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "parser/iterator/value_iterator.h"
#include "sipmsg/sipmsg.h"

#define CMSC_FOREACH_SCHEME_MANDATORY(scheme_ptr, index_var, field_ptr)        \
  for ((index_var) = 0, (field_ptr) = &(scheme_ptr)->mandatory.fields[0];      \
       (index_var) < (scheme_ptr)->mandatory.len;                              \
       ++(index_var), ++(field_ptr))

#define CMSC_FOREACH_SCHEME_OPTIONAL(scheme_ptr, index_var, field_ptr)         \
  for ((index_var) = 0, (field_ptr) = &(scheme_ptr)->optional.fields[0];       \
       (index_var) < (scheme_ptr)->optional.len; ++(index_var), ++(field_ptr))

struct cmsc_SchemeField {
  const char *id;

  bool is_list; // Fields wich can occur multiple times are
                //   represented as lists.

  bool (*is_field_func)(         // This function is optional if none
      const uint32_t buffer_len, // provided default one is used.
      const char *buffer);

  cme_error_t (*parse_field_func)(const struct cmsc_ValueIterator *value_iter,
                                  cmsc_sipmsg_t msg);

  void *generate_field_func;
};

struct cmsc_SchemeFields {
  struct cmsc_SchemeField *fields;
  uint32_t size;
  uint32_t len;
};

struct cmsc_Scheme {
  enum cmsc_SupportedMessages supmsg;
  struct cmsc_SchemeFields mandatory;
  struct cmsc_SchemeFields optional;
};

cme_error_t cmsc_scheme_init(const enum cmsc_SupportedMessages supmsg,
                             struct cmsc_Scheme *scheme);

void cmsc_scheme_destroy(struct cmsc_Scheme *scheme);

cme_error_t
cmsc_scheme_add_mandatory_field(const struct cmsc_SchemeField *field,
                                struct cmsc_Scheme *scheme);

cme_error_t cmsc_scheme_add_optional_field(const struct cmsc_SchemeField *field,
                                           struct cmsc_Scheme *scheme);

#endif // C_MINILIB_SIP_CODEC_SCHEME_H

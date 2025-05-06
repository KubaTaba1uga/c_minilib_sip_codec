#ifndef C_MINILIB_SIP_CODEC_MESSAGE_H
#define C_MINILIB_SIP_CODEC_MESSAGE_H

#include <stdbool.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

// We need to assign fields to 2^N values, to represent seperate
//  bits in `present_mask` field.
enum cmsc_SipField {
  cmsc_SipField_IS_REQUEST = 1,
  cmsc_SipField_SIP_STATUS_CODE = 2,
  cmsc_SipField_SIP_METHOD = 4,
  cmsc_SipField_SIP_MSG_TYPE = 8,
  cmsc_SipField_SIP_PROTO_VER = 16,
  cmsc_SipField_VIA_L = 32,
};

cme_error_t cmsc_message_create(struct cmsc_SipMessage **message,
                                uint32_t buffer_size);

void cmsc_message_destroy(struct cmsc_SipMessage **message);

static inline void
cmsc_message_mark_field_present(struct cmsc_SipMessage *message,
                                enum cmsc_SipField field) {
  message->present_mask = message->present_mask | field;
}

static inline bool
cmsc_message_is_field_present(struct cmsc_SipMessage *message,
                              enum cmsc_SipField field) {
  return message->present_mask & field;
}

#endif // C_MINILIB_SIP_CODEC_SCHEME_H

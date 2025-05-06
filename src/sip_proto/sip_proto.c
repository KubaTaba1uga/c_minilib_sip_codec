#include <stdint.h>

#include <c_minilib_error.h>
#include <string.h>

#include "c_minilib_sip_codec.h"
#include "sip_proto/common_sip_proto.h"
#include "sip_proto/sip_modules/invite.h"
#include "sip_proto/sip_modules/ok_200.h"
#include "sip_proto/sip_proto.h"

/* We are mapping msg types to schemes to allow for O(1) fetch once
 * msg type is known. */
static struct cmsc_Scheme *cmsc_schemes_map[] = {
    [cmsc_SipMsgType_INVITE] = NULL, [cmsc_SipMsgType_200_OK] = NULL,
    // Add more message types here
};

// This array is prepared for iteration better than map
static struct cmsc_Scheme **cmsc_schemes = NULL;
static uint32_t cmsc_schemes_len = 0;

cme_error_t cmsc_sip_proto_init(void) {
#define CMSC_SIP_PROTO_INIT(type, init_func)                                   \
  if (!cmsc_schemes_map[type] && (err = init_func(&cmsc_schemes_map[type]))) { \
    goto error_out;                                                            \
  }

  if (cmsc_schemes) {
    return 0;
  }

  cme_error_t err;

  // We need to initialize possible holes between ptrs to NULL.
  memset(cmsc_schemes_map, 0,
         sizeof(cmsc_schemes_map) / sizeof(struct cmsc_Scheme *));

  CMSC_SIP_PROTO_INIT(cmsc_SipMsgType_INVITE, cmsc_sip_proto_invite_init);
  CMSC_SIP_PROTO_INIT(cmsc_SipMsgType_200_OK, cmsc_sip_proto_ok_200_init);
  // Add more message types here

  // Once all schemes are initialized we are creating helper for iteration over
  // schemes to do not need to consider gaps between ptrs.
  cmsc_schemes = malloc(sizeof(cmsc_schemes_map));
  if (!cmsc_schemes) {
    err = cme_error(ENOMEM, "Unable to allocate memory for `cmsc_schemes`");
    goto error_out;
  }

  cmsc_schemes_len = 0;
  for (uint32_t i = 0;
       i < sizeof(cmsc_schemes_map) / sizeof(struct cmsc_Scheme *); i++) {
    if (!cmsc_schemes_map[i]) {
      continue;
    }

    cmsc_schemes[cmsc_schemes_len++] = cmsc_schemes_map[i];
  }

  return 0;

error_out:
  cmsc_sip_proto_destroy();
  return cme_return(err);
};

void cmsc_sip_proto_destroy(void) {
#define CMSC_SIP_PROTO_DESTROY(type, destroy_func)                             \
  if (cmsc_schemes_map[type]) {                                                \
    destroy_func(&cmsc_schemes_map[type]);                                     \
    cmsc_schemes_map[type] = NULL;                                             \
  };

  CMSC_SIP_PROTO_DESTROY(cmsc_SipMsgType_INVITE, cmsc_sip_proto_invite_destroy);
  CMSC_SIP_PROTO_DESTROY(cmsc_SipMsgType_200_OK, cmsc_sip_proto_ok_200_destroy);
  // Add more message types here

  free(cmsc_schemes);
  cmsc_schemes = NULL;
  cmsc_schemes_len = 0;
};

struct cmsc_SipLine {
  uint32_t len;
  const char *start;
  const char *end;
};

cme_error_t cmsc_sip_proto_parse(const uint32_t n, const char *buffer,
                                 cmsc_sipmsg_t msg) {
  cme_error_t err;

  if ((err = cmsc_sip_proto_parse_first_line(
           cmsc_schemes_len, (const struct cmsc_Scheme **)cmsc_schemes, n,
           buffer, msg))) {
    goto error_out;
  };

  // Find end of SIP message
  // produce lines split by CLRF, unitl end of msg reached

  struct cmsc_Scheme *scheme = cmsc_schemes_map[msg->sip_msg_type];

  for (uint32_t __i__ = 0; __i__ < scheme->mandatory.len; ++__i__) {
    struct cmsc_SchemeField scheme_field = scheme->mandatory.fields[__i__];
    struct cmsc_SipLine line = {.start = buffer, .end = NULL, .len = 0};
    struct cmsc_SipLine key = {.start = NULL, .end = NULL, .len = 0};
    struct cmsc_SipLine value = {.start = NULL, .end = NULL, .len = 0};
    bool is_field_match = false;

    while (line.start) {
      line.end = strstr(line.start, "\r\n");
      if (!line.end) {
        break;
      }

      line.len = line.end - line.start;

      if (line.len == 0) {
        break;
        // This is body, two \r\n in the row
      }

      printf("line.start=%.*s\n", line.len, line.start);

      key.start = line.start;
      key.end = strstr(key.start, ":");
      if (!key.end || key.end > line.end) {
        continue;
      }

      key.len = key.end - key.start;

      if (scheme_field.is_field_func) { // To-do add default is field func
        is_field_match = scheme_field.is_field_func(key.len, key.start);
      } else {
        is_field_match =
            cmsc_default_is_field_func(key.len, key.start, scheme_field.id);
      }

      if (is_field_match) {
        value.start = key.end + 1;

        // According to RFC 3261 7.3.1 Header Field Format
        //  there can be multiple spaces before value.
        while (isblank(*value.start)) {
          value.start++;
        }

        value.end = line.end;
        value.len = value.end - value.start;

        if ((err =
                 scheme_field.parse_field_func(value.len, value.start, msg))) {
          goto error_out;
        };

        // We need to verify that field is not a list
        //  before breaking the loop.
        break;
      }

      line.start = line.end + 2;
    }

    if (!is_field_match) {
      err = cme_errorf(ENOENT, "Can't find mandatory field `%s` in scheme `%s`",
                       scheme_field.id, scheme->sip_msg_type_str);
      goto error_out;
    }
  };

  return 0;

error_out:
  return cme_return(err);
}

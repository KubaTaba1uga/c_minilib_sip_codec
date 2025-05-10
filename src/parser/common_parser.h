/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_COMMON_PARSER_H
#define C_MINILIB_SIP_CODEC_COMMON_PARSER_H

#include <ctype.h>

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"

static inline bool cmsc_default_is_field_func(const uint32_t buffer_len,
                                              const char *buffer,
                                              const char *id) {
  uint32_t id_len = strlen(id);
  if (id_len > buffer_len) {
    return false;
  }

  id_len++;

  char tmp_buffer[id_len];
  char tmp_id[id_len];
  strncpy(tmp_buffer, buffer, id_len);
  tmp_buffer[id_len - 1] = 0;
  strncpy(tmp_id, id, id_len);
  tmp_id[id_len - 1] = 0;

  // According to RFC 3261 7.3.1 case doesn't matter.
  for (uint32_t i = 0; i < id_len; i++) {
    tmp_buffer[i] = tolower(tmp_buffer[i]);
    tmp_id[i] = tolower(tmp_id[i]);
  }

  return strcmp(tmp_id, tmp_buffer) == 0;
};

#endif

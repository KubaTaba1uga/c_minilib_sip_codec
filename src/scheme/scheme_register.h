/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_SIP_CODEC_SCHEME_REGISTER_H
#define C_MINILIB_SIP_CODEC_SCHEME_REGISTER_H

#include <c_minilib_error.h>

#include "c_minilib_sip_codec.h"
#include "scheme/scheme.h"

cme_error_t cmsc_schemes_register_init(void);
void cmsc_schemes_register_destroy(void);

cme_error_t cmsc_schemes_register_add_scheme(struct cmsc_Scheme scheme);
struct cmsc_Scheme *
cmsc_schemes_register_get_scheme(enum cmsc_SupportedMessages supmsg);

#endif

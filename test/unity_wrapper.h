/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#include <unity.h>

#define MYTEST_ASSERT_EQUAL_STRING_LEN(expected, actual, actual_len)           \
  TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, strlen(expected));            \
  TEST_ASSERT_EQUAL(strlen(expected), actual_len);

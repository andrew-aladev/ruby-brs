// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_ERROR_H)
#define BRS_EXT_ERROR_H

#include "ruby.h"

NORETURN(void brs_ext_raise_error(const char* name, const char* description));

#endif // BRS_EXT_ERROR_H

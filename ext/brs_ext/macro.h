// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_MACRO_H)
#define BRS_EXT_MACRO_H

#if defined(__GNUC__)
#define BRS_EXT_UNUSED(x) x __attribute__((__unused__))
#else
#define BRS_EXT_UNUSED(x) x
#endif

#endif // BRS_EXT_MACRO_H

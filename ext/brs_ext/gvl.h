// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_GVL_H)
#define BRS_EXT_GVL_H

#ifdef HAVE_RB_THREAD_CALL_WITHOUT_GVL

#include "ruby/thread.h"

#define BRS_EXT_GVL_WRAP(with_gvl, function, data)                         \
  if (with_gvl) {                                                          \
    function((void*) data);                                                \
  } else {                                                                 \
    rb_thread_call_without_gvl(function, (void*) data, RUBY_UBF_IO, NULL); \
  }

#else

#define BRS_EXT_GVL_WRAP(_with_gvl, function, data) function((void*) data);

#endif

#endif // BRS_EXT_GVL_H

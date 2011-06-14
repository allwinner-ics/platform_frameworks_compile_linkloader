#include "rsl_assert.h"

#include <llvm/Support/raw_ostream.h>

#include <stdlib.h>

#ifdef __arm__
#define LOG_TAG "bcc"
#include <cutils/log.h>
#endif

extern "C" void ASSERT_FAILED(char const *file,
                              unsigned line,
                              char const *expr) {
#ifndef __arm__
  llvm::errs() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
  llvm::errs() << "rslAssert [" << file << ":" << line << "] " << expr << "\n";
  llvm::errs() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
#else
  LOGE("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  LOGE("rslAssert [%s:%d] %s\n", file, line, expr);
  LOGE("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif

  abort();
}

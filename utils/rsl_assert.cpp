/*
 * Copyright 2011, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

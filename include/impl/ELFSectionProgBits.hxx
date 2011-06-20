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

#ifndef ELF_SECTION_PROGBITS_HXX
#define ELF_SECTION_PROGBITS_HXX

#include "ELFTypes.h"
#include "StubLayout.h"

#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include "utils/raw_ostream.h"

#include <string.h>

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionProgBits<Bitwidth> *
ELFSectionProgBits<Bitwidth>::read(Archiver &AR,
                                   ELFObjectTy *owner,
                                   ELFSectionHeaderTy const *sh) {

  llvm::OwningPtr<ELFSectionProgBits> result(new ELFSectionProgBits());

  bool is_text_section = strcmp(sh->getName(), ".text") == 0;

  if (!is_text_section) {
    if (!result->chunk.allocate(sh->getSize())) {
      return NULL;
    }

  } else {
#ifdef __arm__
    StubLayout *stubs = owner->getStubLayout();

    // Count the extern function symbol
    ELFSectionSymTabTy const *symtab =
      static_cast<ELFSectionSymTabTy *>(owner->getSectionByName(".symtab"));

    size_t func_count = symtab->getExternFuncCount();

    // Calculate additional stub size
    size_t stub_size = stubs->calcStubTableSize(func_count);

    // Allocate text section
    if (!result->chunk.allocate(sh->getSize() + stub_size)) {
      return NULL;
    }

    stubs->initStubTable(result->chunk.getBuffer()+sh->getSize(), func_count);
#else
    // Allocate text section
    if (!result->chunk.allocate(sh->getSize())) {
      return NULL;
    }
#endif
  }

  result->sh = sh;

  if (!result->serialize(AR)) {
    // Unable to read the progbits section.
    return NULL;
  }

  return result.take();
}

#endif // ELF_SECTION_PROGBITS_HXX

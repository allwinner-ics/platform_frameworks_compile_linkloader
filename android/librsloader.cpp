#include "librsloader.h"

#include "ELFObject.h"
#include "ELFSectionSymTab.h"
#include "ELFSymbol.h"

#include "utils/serialize.h"

#define LOG_TAG "bcc"
#include "cutils/log.h"

#include <llvm/ADT/OwningPtr.h>
#include <elf.h>

static inline RSExecRef wrap(ELFObject<32> *object) {
  return reinterpret_cast<RSExecRef>(object);
}

static inline ELFObject<32> *unwrap(RSExecRef object) {
  return reinterpret_cast<ELFObject<32> *>(object);
}

extern "C" RSExecRef
rsloaderCreateExec(unsigned char const *buf,
                   size_t buf_size,
                   void *(*find_symbol)(void *, char const *),
                   void *find_symbol_context) {

  ArchiveReaderLE AR(buf, buf_size);

  llvm::OwningPtr<ELFObject<32> > object(ELFObject<32>::read(AR));
  if (!object) {
    LOGE("Unable to load the ELF object.");
    return NULL;
  }

  //object->print();
  object->relocate(find_symbol, find_symbol_context);

  return wrap(object.take());
}

extern "C" void rsloaderDisposeExec(RSExecRef object) {
  delete unwrap(object);
}

extern "C" void *rsloaderGetSymbolAddress(RSExecRef object_,
                                          char const *name) {
  ELFObject<32> *object = unwrap(object_);

  ELFSectionSymTab<32> *symtab =
    static_cast<ELFSectionSymTab<32> *>(object->getSectionByName(".symtab"));

  if (!symtab) {
    return NULL;
  }

  ELFSymbol<32> *symbol = symtab->getByName(name);

  if (!symbol) {
    LOGE("Symbol not found: %s\n", name);
    return NULL;
  }

  return symbol->getAddress(false);
}

extern "C" size_t rsloaderGetSymbolSize(RSExecRef object_, char const *name) {
  ELFObject<32> *object = unwrap(object_);

  ELFSectionSymTab<32> *symtab =
    static_cast<ELFSectionSymTab<32> *>(object->getSectionByName(".symtab"));

  if (!symtab) {
    return NULL;
  }

  ELFSymbol<32> *symbol = symtab->getByName(name);

  if (!symbol) {
    LOGE("Symbol not found: %s\n", name);
    return NULL;
  }

  return (size_t)symbol->getSize();
}

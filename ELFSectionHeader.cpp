#include "ELFSectionHeader.h"

#include <elf.h>

char const *ELFSectionHeaderHelperMixin::getSectionTypeStr(uint32_t type) {
  switch (type) {
    default: return "(UNKNOWN)";

#define CASE(TYPE) \
    case SHT_##TYPE: return #TYPE;

    // General section header type
    CASE(NULL) CASE(PROGBITS) CASE(SYMTAB) CASE(STRTAB) CASE(RELA) CASE(HASH)
    CASE(DYNAMIC) CASE(NOTE) CASE(NOBITS) CASE(REL) CASE(SHLIB)
    CASE(DYNSYM) CASE(INIT_ARRAY) CASE(FINI_ARRAY) CASE(PREINIT_ARRAY)
    CASE(GROUP) CASE(SYMTAB_SHNDX) CASE(LOOS) CASE(HIOS) CASE(LOPROC)
    CASE(HIPROC) CASE(LOUSER) CASE(HIUSER)

    // ARM-specific section header type
    CASE(ARM_EXIDX) CASE(ARM_PREEMPTMAP) CASE(ARM_ATTRIBUTES)

#undef CASE
  }
}

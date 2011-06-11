#ifndef ELF_SECTION_HEADER_TABLE_H
#define ELF_SECTION_HEADER_TABLE_H

#include "ELFTypes.h"

#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

template <unsigned Bitwidth>
class ELFSectionHeaderTable {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

private:
  std::vector<ELFSectionHeaderTy *> table;

private:
  ELFSectionHeaderTable() {
  }

public:
  ~ELFSectionHeaderTable();

  template <typename Archiver>
  static ELFSectionHeaderTableTy *read(Archiver &AR, ELFObjectTy *owner);

  ELFSectionHeaderTy const *operator[](size_t i) const {
    return table[i];
  }

  ELFSectionHeaderTy *operator[](size_t i) {
    return table[i];
  }

  ELFSectionHeaderTy const *getByName(const std::string &str) const;
  ELFSectionHeaderTy *getByName(const std::string &str);

  void print() const;
};

#include "impl/ELFSectionHeaderTable.hxx"

#endif // ELF_SECTION_HEADER_TABLE_H

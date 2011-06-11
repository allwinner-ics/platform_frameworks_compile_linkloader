#ifndef ELF_SECTION_HEADER_TABLE_H
#define ELF_SECTION_HEADER_TABLE_H

#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

template <unsigned Bitwidth> class ELFObject;

template <unsigned Bitwidth>
class ELFSectionHeaderTable {
private:
  std::vector<ELFSectionHeader<Bitwidth> *> table;

private:
  ELFSectionHeaderTable() {
  }

public:
  ~ELFSectionHeaderTable();

  template <typename Archiver>
  static ELFSectionHeaderTable<Bitwidth> *
  read(Archiver &AR, ELFObject<Bitwidth> *owner);

  ELFSectionHeader<Bitwidth> const *operator[](size_t i) const {
    return table[i];
  }

  ELFSectionHeader<Bitwidth> *operator[](size_t i) {
    return table[i];
  }

  ELFSectionHeader<Bitwidth> const *getByName(const std::string &str) const;
  ELFSectionHeader<Bitwidth> *getByName(const std::string &str);

  void print() const;
};

#include "impl/ELFSectionHeaderTable.hxx"

#endif // ELF_SECTION_HEADER_TABLE_H

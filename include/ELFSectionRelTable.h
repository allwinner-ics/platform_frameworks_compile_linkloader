#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include <string>
#include <vector>

#include "ELFSection.h"

template <unsigned Bitwidth> class ELFReloc;

template <unsigned Bitwidth>
class ELFSectionRelTable : public ELFSection<Bitwidth> {
private:
  std::vector<ELFReloc<Bitwidth> *> rel_table;

private:
  ELFSectionRelTable() { }

public:
  virtual ~ELFSectionRelTable();

  virtual void print() const;

  template <typename Archiver>
  static ELFSectionRelTable *
  read(Archiver &AR,
       ELFSectionHeader<Bitwidth> const *sh);

  size_t size() const {
    return rel_table.size();
  }

  ELFReloc<Bitwidth> const *operator[](size_t index) const {
    return rel_table[index];
  }

  ELFReloc<Bitwidth> *operator[](size_t index) {
    return rel_table[index];
  }
};

#include "impl/ELFSectionRelTable.hxx"

#endif // ELF_SECTION_REL_TABLE_H

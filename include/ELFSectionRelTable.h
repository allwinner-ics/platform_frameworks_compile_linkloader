#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include "ELFTypes.h"
#include "ELFSection.h"

#include <string>
#include <vector>

template <unsigned Bitwidth>
class ELFSectionRelTable : public ELFSection<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

private:
  std::vector<ELFRelocTy *> table;

private:
  ELFSectionRelTable() { }

public:
  virtual ~ELFSectionRelTable();

  virtual void print() const;

  template <typename Archiver>
  static ELFSectionRelTable *read(Archiver &AR, ELFSectionHeaderTy const *sh);

  size_t size() const {
    return table.size();
  }

  ELFRelocTy const *operator[](size_t index) const {
    return table[index];
  }

  ELFRelocTy *operator[](size_t index) {
    return table[index];
  }
};

#include "impl/ELFSectionRelTable.hxx"

#endif // ELF_SECTION_REL_TABLE_H

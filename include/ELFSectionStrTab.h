#ifndef ELF_SECTION_STRTAB_H
#define ELF_SECTION_STRTAB_H

#include "ELFTypes.h"
#include "ELFSection.h"

#include <vector>

template <unsigned Bitwidth>
class ELFSectionStrTab : public ELFSection<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

private:
  ELFSectionHeaderTy const *section_header;
  std::vector<char> buf;

private:
  ELFSectionStrTab() { }

public:
  template <typename Archiver>
  static ELFSectionStrTab *read(Archiver &AR, ELFSectionHeaderTy const *sh);

  virtual void print() const;

  char const *operator[](size_t index) const {
    return &*buf.begin() + index;
  }
};

#include "impl/ELFSectionStrTab.hxx"

#endif // ELF_SECTION_STRTAB_H

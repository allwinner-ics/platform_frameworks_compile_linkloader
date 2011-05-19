#ifndef ELF_SECTION_HEADER_TABLE_H
#define ELF_SECTION_HEADER_TABLE_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include <assert.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth>
class ELFSectionHeaderTable {
private:
  ELFObject<Bitwidth> *owner;
  std::vector<boost::shared_ptr<ELFSectionHeader<Bitwidth> > > table;

private:
  ELFSectionHeaderTable() {
  }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFSectionHeaderTable<Bitwidth> >
  read(Archiver &AR, ELFObject<Bitwidth> *owner);

  ELFSectionHeader<Bitwidth> const *operator[](size_t i) const {
    return table[i].get();
  }

  ELFSectionHeader<Bitwidth> *operator[](size_t i) {
    return table[i].get();
  }

  void print();
};


#include "ELFObject.h"
#include "ELFHeader.h"
#include "ELFSectionHeader.h"
#include "ELFSection.h"
#include "ELFTypes.h"


template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSectionHeaderTable<Bitwidth> >
ELFSectionHeaderTable<Bitwidth>::read(Archiver &AR,
                                      ELFObject<Bitwidth> *owner) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return boost::shared_ptr<ELFSectionHeaderTable>();
  }

  // Allocate a new section header table and assign the owner.
  boost::shared_ptr<ELFSectionHeaderTable> tab(new ELFSectionHeaderTable());
  tab->owner = owner;

  // Get ELF header
  ELFHeader<Bitwidth> const *header = owner->getHeader();

  assert(header->getSectionHeaderEntrySize() >=
         TypeTraits<ELFSectionHeader<Bitwidth> >::size);

  size_t pending = TypeTraits<ELFSectionHeader<Bitwidth> >::size -
                   header->getSectionHeaderEntrySize();

  // Seek to the address of section header
  AR.seek(header->getSectionHeaderTableOffset(), true);

  for (size_t i = 0; i < header->getSectionHeaderNum(); ++i) {
    boost::shared_ptr<ELFSectionHeader<Bitwidth> > sh(
      ELFSectionHeader<Bitwidth>::read(AR, i));

    if (!sh) {
      // Something wrong while reading the section header.
      return boost::shared_ptr<ELFSectionHeaderTable>();
    }

    AR.seek(pending);
    tab->table.push_back(sh);
  }

  return tab;
}

template <size_t Bitwidth>
inline void ELFSectionHeaderTable<Bitwidth>::print() {
  using namespace std;
  using namespace term;
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;
  cout << light::white()
       << "ELF Section Header Table" << normal() << endl;

  for (size_t i = 0; i < table.size(); ++i) {
    table[i]->print();
  }

  cout << setw(79) << setfill('=') << '=' << endl << endl;
}

#endif // ELF_SECTION_HEADER_TABLE_H

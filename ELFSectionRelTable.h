#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include "ELFSection.h"

#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <vector>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionHeader;
template <size_t Bitwidth> class ELFSectionRel;

template <size_t Bitwidth>
class ELFSectionRelTable : public ELFSection<Bitwidth> {
public:
  typedef ELFSectionRel<Bitwidth> Rel;

private:
  std::vector<boost::shared_ptr<Rel> > rel_table;

private:
  ELFSectionRelTable() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFSectionRelTable>
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return rel_table.size();
  }

  Rel const *operator[](size_t index) const {
    return rel_table[index].get();
  }
};


//==================Inline Member Function Definition==========================

#include "ELFSectionRel.h"
#include "ELFSectionHeader.h"
#include "ELFTypes.h"

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSectionRelTable<Bitwidth> >
ELFSectionRelTable<Bitwidth>::read(Archiver &AR,
                                 ELFObject<Bitwidth> *owner,
                                 ELFSectionHeader<Bitwidth> const *sh) {
  using namespace boost;

  shared_ptr<ELFSectionRelTable> st(new ELFSectionRelTable());

  // Assert that entry size will be the same as standard.
  assert(sh->getEntrySize() ==
         TypeTraits<ELFSectionRel<Bitwidth> >::size);

  // Read all symbol table entry

  size_t tsize = sh->getSize() / sh->getEntrySize();
  for (size_t i = 0; i < tsize; ++i) {
    // Seek to symbol table start address
    AR.seek(sh->getOffset() + i*sh->getEntrySize(), true);
    st->rel_table.push_back(Rel::read(AR, owner, i));
  }

  if (!AR) {
    // Unable to read the string table.
    return shared_ptr<ELFSectionRelTable>();
  }

  return st;
}

template <size_t Bitwidth>
inline void ELFSectionRelTable<Bitwidth>::print() const {
  using namespace std;
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;
  cout << light::white() << "Relocation Table" << normal() << endl;

  for (size_t i = 0; i < this->size(); ++i) {
      (*this)[i]->print();
  }

  cout << setw(79) << setfill('=') << '=' << endl << endl;
}

#endif // ELF_SECTION_REL_TABLE_H

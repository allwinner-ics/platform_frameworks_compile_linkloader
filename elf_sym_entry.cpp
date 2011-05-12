#include "elf_sym_entry.h"

#include "elf_object.h"
#include "elf_header.h"
#include "elf_strtab.h"
#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>

#include <iomanip>
#include <iostream>
#include <string>

#include <elf.h>

using namespace boost;
using namespace serialization;
using namespace std;


class elf_sym_entry_32 : public elf_sym_entry {
private:
  Elf32_Word st_name;
  Elf32_Addr st_value;
  Elf32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half st_shndx;

public:
  elf_sym_entry_32(elf_object const &obj) : elf_sym_entry(obj) {
  }

  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint32_t get_name_index() const;
  virtual uint8_t get_type() const;
  virtual uint8_t get_binding_attribute() const;
  virtual uint8_t get_visibility() const;
  virtual uint16_t get_section_index() const;
  virtual uint64_t get_value() const;
  virtual uint64_t get_size() const;
};

template <typename Archiver>
void elf_sym_entry_32::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf32_Sym));

  AR & st_name;
  AR & st_value;
  AR & st_size;
  AR & st_info;
  AR & st_other;
  AR & st_shndx;

  AR.epilogue(sizeof(Elf32_Sym));
}

uint32_t elf_sym_entry_32::get_name_index() const{
  return st_name;
}
uint8_t elf_sym_entry_32::get_type() const{
  return ELF32_ST_TYPE(st_info);
}
uint8_t elf_sym_entry_32::get_binding_attribute() const{
  return ELF32_ST_BIND(st_info);
}
uint8_t elf_sym_entry_32::get_visibility() const{
  return ELF32_ST_VISIBILITY(st_other);
}
uint16_t elf_sym_entry_32::get_section_index() const{
  return st_shndx;
}
uint64_t elf_sym_entry_32::get_value() const{
  return st_value;
}
uint64_t elf_sym_entry_32::get_size() const{
  return st_size;
}

//=============================================================================


class elf_sym_entry_64 : public elf_sym_entry {
private:
  Elf64_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf64_Half st_shndx;
  Elf64_Addr st_value;
  Elf64_Xword st_size;

public:
  elf_sym_entry_64(elf_object const &obj) : elf_sym_entry(obj) {
  }

  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint32_t get_name_index() const;
  virtual uint8_t get_type() const;
  virtual uint8_t get_binding_attribute() const;
  virtual uint8_t get_visibility() const;
  virtual uint16_t get_section_index() const;
  virtual uint64_t get_value() const;
  virtual uint64_t get_size() const;
};

template <typename Archiver>
void elf_sym_entry_64::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf64_Sym));

  AR & st_name;
  AR & st_info;
  AR & st_other;
  AR & st_shndx;
  AR & st_value;
  AR & st_size;

  AR.epilogue(sizeof(Elf64_Sym));
}

uint32_t elf_sym_entry_64::get_name_index() const {
  return st_name;
}
uint8_t elf_sym_entry_64::get_type() const {
  return ELF64_ST_TYPE(st_info);
}
uint8_t elf_sym_entry_64::get_binding_attribute() const {
  return ELF64_ST_BIND(st_info);
}
uint8_t elf_sym_entry_64::get_visibility() const {
  return ELF64_ST_VISIBILITY(st_other);
}
uint16_t elf_sym_entry_64::get_section_index() const {
  return st_shndx;
}
uint64_t elf_sym_entry_64::get_value() const {
  return st_value;
}
uint64_t elf_sym_entry_64::get_size() const {
  return st_size;
}


//=============================================================================

char const *elf_sym_entry::get_name() const {
  return owner.get_symbol_str_tab()[get_name_index()];
}

template <typename Archiver>
shared_ptr<elf_sym_entry>
elf_sym_entry::read_32(Archiver &AR, elf_object const &obj) {
  shared_ptr<elf_sym_entry_32> result(new elf_sym_entry_32(obj));

  // Read the ELF symbol entry from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_sym_entry_32>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_sym_entry>
elf_sym_entry::read_64(Archiver &AR, elf_object const &obj) {
  shared_ptr<elf_sym_entry_64> result(new elf_sym_entry_64(obj));

  // Read the ELF symbol entry from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_sym_entry_64>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_sym_entry>
elf_sym_entry::read(Archiver &AR, elf_object const &obj) {
  shared_ptr<elf_sym_entry> result =
    obj.get_header().is_64bit() ? read_64(AR, obj) : read_32(AR, obj);

  if (!result) {
    return shared_ptr<elf_sym_entry>();
  }

  return result;
}

template shared_ptr<elf_sym_entry>
elf_sym_entry::read(archive_reader_le &, elf_object const &);

template shared_ptr<elf_sym_entry>
elf_sym_entry::read(archive_reader_be &, elf_object const &);


//=============================================================================

char const *elf_sym_entry::get_type_name(uint8_t type) {
  switch (type) {
    default: return "(UNKNOWN)";

#define CASE(TYPE) \
    case STT_##TYPE: return #TYPE;

    CASE(NOTYPE)
    CASE(OBJECT)
    CASE(FUNC)
    CASE(SECTION)
    CASE(FILE)
    CASE(COMMON)
    CASE(TLS)
    CASE(LOOS)
    CASE(HIOS)
    CASE(LOPROC)
    CASE(HIPROC)

#undef CASE
  }
}

char const *elf_sym_entry::get_binding_attribute_name(uint8_t type) {
  switch (type) {
    default: return "(UNKNOWN)";

#define CASE(TYPE) \
    case STB_##TYPE: return #TYPE;

    CASE(LOCAL)
    CASE(GLOBAL)
    CASE(WEAK)
    CASE(LOOS)
    CASE(HIOS)
    CASE(LOPROC)
    CASE(HIPROC)

#undef CASE
  }
}

char const *elf_sym_entry::get_visibility_name(uint8_t type) {
  switch (type) {
    default: return "(UNKNOWN)";

#define CASE(TYPE) \
    case STV_##TYPE: return #TYPE;

    CASE(DEFAULT)
    CASE(INTERNAL)
    CASE(HIDDEN)
    CASE(PROTECTED)

#undef CASE
  }
}

void elf_sym_entry::print() const {
  using namespace term::color;

  cout << setw(20) << get_name() <<
          setw(10) << get_type_name(get_type()) <<
          setw(10) << get_binding_attribute_name(get_binding_attribute()) <<
          setw(15) << get_visibility_name(get_visibility()) <<
          setw(10) << get_section_index() <<
          setw(7) << get_value() <<
          setw(7) << get_size() <<
          endl;
}


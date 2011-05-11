#include "elf_section_header.h"

#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>

#include <iomanip>
#include <iostream>

#include <elf.h>

using namespace boost;
using namespace serialization;
using namespace std;


class elf_section_header_32 : public elf_section_header {
private:
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual char const* get_name() const;
  virtual uint32_t get_type() const;
  virtual uint64_t get_flags() const;
  virtual uint64_t get_address() const;
  virtual uint64_t get_offset() const;
  virtual uint64_t get_size() const;
  virtual uint32_t get_link() const;
  virtual uint32_t get_extra_info() const;
  virtual uint64_t get_address_align() const;
  virtual uint64_t get_entry_size() const;
};

template <typename Archiver>
void elf_section_header_32::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf32_Shdr));

  AR & sh_name;
  AR & sh_type;
  AR & sh_flags;
  AR & sh_addr;
  AR & sh_offset;
  AR & sh_size;
  AR & sh_link;
  AR & sh_info;
  AR & sh_addralign;
  AR & sh_entsize;

  AR.epilogue(sizeof(Elf32_Shdr));
}

char const* elf_section_header_32::get_name() const {
  return "Oops! I don't know, not finished yet.";
}

uint32_t elf_section_header_32::get_type() const {
  return sh_type;
}

uint64_t elf_section_header_32::get_flags() const {
  return sh_flags;
}

uint64_t elf_section_header_32::get_address() const {
  return sh_addr;
}

uint64_t elf_section_header_32::get_offset() const {
  return sh_offset;
}

uint64_t elf_section_header_32::get_size() const {
  return sh_size;
}

uint32_t elf_section_header_32::get_link() const {
	return sh_link;
}

uint32_t elf_section_header_32::get_extra_info() const {
	return sh_info;
}

uint64_t elf_section_header_32::get_address_align() const {
	return sh_addralign;
}

uint64_t elf_section_header_32::get_entry_size() const {
	return sh_entsize;
}


//=============================================================================


class elf_section_header_64 : public elf_section_header {
private:
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual char const* get_name() const;
  virtual uint32_t get_type() const;
  virtual uint64_t get_flags() const;
  virtual uint64_t get_address() const;
  virtual uint64_t get_offset() const;
  virtual uint64_t get_size() const;
  virtual uint32_t get_link() const;
  virtual uint32_t get_extra_info() const;
  virtual uint64_t get_address_align() const;
  virtual uint64_t get_entry_size() const;
};

template <typename Archiver>
void elf_section_header_64::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf64_Shdr));

  AR & sh_name;
  AR & sh_type;
  AR & sh_flags;
  AR & sh_addr;
  AR & sh_offset;
  AR & sh_size;
  AR & sh_link;
  AR & sh_info;
  AR & sh_addralign;
  AR & sh_entsize;

  AR.epilogue(sizeof(Elf64_Shdr));
}

char const* elf_section_header_64::get_name() const {
  return "I don't know.";
}

uint32_t elf_section_header_64::get_type() const {
  return sh_type;
}

uint64_t elf_section_header_64::get_flags() const {
  return sh_flags;
}

uint64_t elf_section_header_64::get_address() const {
  return sh_addr;
}

uint64_t elf_section_header_64::get_offset() const {
  return sh_offset;
}

uint64_t elf_section_header_64::get_size() const {
  return sh_size;
}

uint32_t elf_section_header_64::get_link() const {
	return sh_link;
}

uint32_t elf_section_header_64::get_extra_info() const {
	return sh_info;
}

uint64_t elf_section_header_64::get_address_align() const {
	return sh_addralign;
}

uint64_t elf_section_header_64::get_entry_size() const {
	return sh_entsize;
}


//=============================================================================

template <typename Archiver>
shared_ptr<elf_section_header> elf_section_header::read_32(Archiver &AR) {
  shared_ptr<elf_section_header_32> result(new elf_section_header_32());

  // Read the ELF header from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_section_header_32>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_section_header> elf_section_header::read_64(Archiver &AR) {
  shared_ptr<elf_section_header_64> result(new elf_section_header_64());

  // Read the ELF header from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_section_header_64>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_section_header> elf_section_header::read(Archiver &AR, bool is_64bit) {
  return is_64bit ? read_64(AR) : read_32(AR);
}

template shared_ptr<elf_section_header> elf_section_header::read(archive_reader_le &, bool);
template shared_ptr<elf_section_header> elf_section_header::read(archive_reader_be &, bool);


//=============================================================================

void elf_section_header::print_header(){
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "Section Header" << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');
  cout << setw(15) << "Name" <<
          setw(6) << "Type" <<
          setw(7) << "Flags" <<
          setw(8) << "Address" <<
          setw(8) << "Offset" <<
          setw(7) << "Size" <<
          setw(7) << "Link" <<
          setw(7) << "Info" <<
          setw(7) << "Align" <<
          setw(7) << "Esize" <<
          endl;
}

void elf_section_header::print_footer(){
  using namespace term::color;

  cout << setw(79) << setfill('=') << '=' << endl;
}

void elf_section_header::print() const {
  using namespace term::color;

  cout << setw(15) << get_name() <<
          setw(6) << get_type() <<
          setw(7) << get_flags() <<
          setw(8) << (void*)get_address() <<
          setw(8) << (void*)get_offset() <<
          setw(7) << get_size() <<
          setw(7) << get_link() <<
          setw(7) << get_extra_info() <<
          setw(7) << get_address_align() <<
          setw(7) << get_entry_size() <<
          endl;
}


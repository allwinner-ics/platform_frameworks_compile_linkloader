#include "elf_header.h"

#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>

#include <iomanip>
#include <iostream>

#include <elf.h>

using namespace boost;
using namespace serialization;
using namespace std;


class elf_header_32 : public elf_header {
private:
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint16_t get_object_type() const;
  virtual uint16_t get_machine() const;
  virtual uint32_t get_version() const;
  virtual uint64_t get_entry_vaddr() const;
  virtual uint64_t get_program_header_table_offset() const;
  virtual uint64_t get_section_header_table_offset() const;
  virtual uint32_t get_flags() const;
  virtual uint16_t get_elf_header_size() const;
  virtual uint16_t get_program_header_entry_size() const;
  virtual uint16_t get_program_header_num() const;
  virtual uint16_t get_section_header_entry_size() const;
  virtual uint16_t get_section_header_num() const;
  virtual uint16_t get_str_section_index() const;
};

template <typename Archiver>
void elf_header_32::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf32_Ehdr));

  AR & e_ident;
  AR & e_type;
  AR & e_machine;
  AR & e_version;
  AR & e_entry;
  AR & e_phoff;
  AR & e_shoff;
  AR & e_flags;
  AR & e_ehsize;
  AR & e_phentsize;
  AR & e_phnum;
  AR & e_shentsize;
  AR & e_shnum;
  AR & e_shstrndx;

  AR.epilogue(sizeof(Elf32_Ehdr));
}

uint16_t elf_header_32::get_object_type() const {
  return e_type;
}

uint16_t elf_header_32::get_machine() const {
  return e_machine;
}

uint32_t elf_header_32::get_version() const {
  return e_version;
}

uint64_t elf_header_32::get_entry_vaddr() const {
  return e_entry;
}

uint64_t elf_header_32::get_program_header_table_offset() const {
  return e_phoff;
}

uint64_t elf_header_32::get_section_header_table_offset() const {
  return e_shoff;
}

uint32_t elf_header_32::get_flags() const {
  return e_flags;
}

uint16_t elf_header_32::get_elf_header_size() const {
  return e_ehsize;
}

uint16_t elf_header_32::get_program_header_entry_size() const {
  return e_phentsize;
}

uint16_t elf_header_32::get_program_header_num() const {
  return e_phnum;
}

uint16_t elf_header_32::get_section_header_entry_size() const {
  return e_shentsize;
}

uint16_t elf_header_32::get_section_header_num() const {
  return e_shnum;
}

uint16_t elf_header_32::get_str_section_index() const {
  return e_shstrndx;
}


//=============================================================================


class elf_header_64 : public elf_header {
private:
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint16_t get_object_type() const;
  virtual uint16_t get_machine() const;
  virtual uint32_t get_version() const;
  virtual uint64_t get_entry_vaddr() const;
  virtual uint64_t get_program_header_table_offset() const;
  virtual uint64_t get_section_header_table_offset() const;
  virtual uint32_t get_flags() const;
  virtual uint16_t get_elf_header_size() const;
  virtual uint16_t get_program_header_entry_size() const;
  virtual uint16_t get_program_header_num() const;
  virtual uint16_t get_section_header_entry_size() const;
  virtual uint16_t get_section_header_num() const;
  virtual uint16_t get_str_section_index() const;
};

template <typename Archiver>
void elf_header_64::serialize(Archiver &AR) {
  AR.prologue(sizeof(Elf64_Ehdr));

  AR & e_ident;
  AR & e_type;
  AR & e_machine;
  AR & e_version;
  AR & e_entry;
  AR & e_phoff;
  AR & e_shoff;
  AR & e_flags;
  AR & e_ehsize;
  AR & e_phentsize;
  AR & e_phnum;
  AR & e_shentsize;
  AR & e_shnum;
  AR & e_shstrndx;

  AR.epilogue(sizeof(Elf64_Ehdr));
}

uint16_t elf_header_64::get_object_type() const {
  return e_type;
}

uint16_t elf_header_64::get_machine() const {
  return e_machine;
}

uint32_t elf_header_64::get_version() const {
  return e_version;
}

uint64_t elf_header_64::get_entry_vaddr() const {
  return e_entry;
}

uint64_t elf_header_64::get_program_header_table_offset() const {
  return e_phoff;
}

uint64_t elf_header_64::get_section_header_table_offset() const {
  return e_shoff;
}

uint32_t elf_header_64::get_flags() const {
  return e_flags;
}

uint16_t elf_header_64::get_elf_header_size() const {
  return e_ehsize;
}

uint16_t elf_header_64::get_program_header_entry_size() const {
  return e_phentsize;
}

uint16_t elf_header_64::get_program_header_num() const {
  return e_phnum;
}

uint16_t elf_header_64::get_section_header_entry_size() const {
  return e_shentsize;
}

uint16_t elf_header_64::get_section_header_num() const {
  return e_shnum;
}

uint16_t elf_header_64::get_str_section_index() const {
  return e_shstrndx;
}


//=============================================================================


int elf_header::get_class() const {
  return e_ident[EI_CLASS];
}

int elf_header::get_endianness() const {
  return e_ident[EI_DATA];
}

int elf_header::get_header_version() const {
  return e_ident[EI_VERSION];
}

int elf_header::get_os_abi() const {
  return e_ident[EI_OSABI];
}

int elf_header::get_abi_version() const {
  return e_ident[EI_ABIVERSION];
}

bool elf_header::is_32bit() const {
  return e_ident[EI_CLASS] == ELFCLASS32;
}

bool elf_header::is_64bit() const {
  return e_ident[EI_CLASS] == ELFCLASS64;
}

bool elf_header::is_big_endian() const {
  return e_ident[EI_DATA] == ELFDATA2MSB;
}

bool elf_header::is_little_endian() const {
  return e_ident[EI_DATA] == ELFDATA2LSB;
}


//=============================================================================


inline bool elf_header::is_valid_magic_word() const {
  return (memcmp(e_ident, "\x7f" "ELF", 4) == 0);
}

inline bool elf_header::is_valid_class() const {
  return (is_32bit() || is_64bit());
}

inline bool elf_header::is_valid_endianness() const {
  return (is_big_endian() || is_little_endian());
}

inline bool elf_header::is_valid_header_version() const {
  return (get_header_version() == EV_CURRENT);
}

inline bool elf_header::is_unused_zeroed_padding() const {
  for (size_t i = EI_PAD; i < EI_NIDENT; ++i) {
    if (e_ident[i] != 0) {
      return false;
    }
  }
  return true;
}

inline bool elf_header::is_valid_elf_ident() const {
  return (is_valid_magic_word() &&
          is_valid_class() &&
          is_valid_endianness() &&
          is_valid_header_version() &&
          is_unused_zeroed_padding());
}

inline bool elf_header::is_compatible_header_size() const {
  if (is_32bit()) {
    return (get_elf_header_size() >= sizeof(Elf32_Ehdr) &&
              (!(get_object_type() == ET_EXEC || get_object_type() == ET_DYN)
               || get_program_header_entry_size() >= sizeof(Elf32_Phdr)) &&
            get_section_header_entry_size() >= sizeof(Elf32_Shdr));
  } else {
    return (get_elf_header_size() >= sizeof(Elf64_Ehdr) &&
              (!(get_object_type() == ET_EXEC || get_object_type() == ET_DYN)
               || get_program_header_entry_size() >= sizeof(Elf64_Phdr)) &&
            get_section_header_entry_size() >= sizeof(Elf64_Shdr));
  }
}

bool elf_header::is_valid_elf_header() const {
  return (is_valid_elf_ident() && is_compatible_header_size());
}


//=============================================================================


template <typename Archiver>
shared_ptr<elf_header> elf_header::read_32(Archiver &AR) {
  shared_ptr<elf_header_32> result(new elf_header_32());

  // Read the ELF header from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_header_32>();
  }

  // Ensure that the ELF header is valid
  if (!result->is_valid_elf_header()) {
    return shared_ptr<elf_header_32>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_header> elf_header::read_64(Archiver &AR) {
  shared_ptr<elf_header_64> result(new elf_header_64());

  // Read the ELF header from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_header_64>();
  }

  // Ensure that the ELF header is valid
  if (!result->is_valid_elf_header()) {
    return shared_ptr<elf_header_64>();
  }

  return result;
}

template <typename Archiver>
shared_ptr<elf_header> elf_header::read(Archiver &AR) {
  unsigned char ident[EI_NIDENT];

  AR.prologue(sizeof(ident));
  AR & ident;
  AR.epilogue(sizeof(ident));
  AR.seek(0, true);

  return (ident[EI_CLASS] == ELFCLASS64) ? read_64(AR) : read_32(AR);
}

template shared_ptr<elf_header> elf_header::read(archive_reader_le &);
template shared_ptr<elf_header> elf_header::read(archive_reader_be &);


//=============================================================================


void elf_header::print() const {
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "ELF Header" << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

  cout << setw(25) << "Class" << " : " << get_class_name(get_class()) << endl;

  cout << setw(25) << "Endianness" << " : "
       << get_endianness_name(get_endianness()) << endl;

  cout << setw(25) << "Header Version" << " : "
       << get_header_version() << endl;

  cout << setw(25) << "OS ABI" << " : "
       << get_os_abi_name(get_os_abi()) << endl;

  cout << setw(25) << "ABI Version" << " : " << get_abi_version() << endl;


  cout << setw(25) << "Object Type" << " : "
       << get_object_type_name(get_object_type()) << endl;

  cout << setw(25) << "Machine" << " : "
       << get_machine_name(get_machine()) << endl;

  cout << setw(25) << "Version" << " : "
       << get_version_name(get_version()) << endl;

  cout << setw(25) << "Entry Address" << " : "
       << setw(16) << setfill('0') << hex << get_entry_vaddr()
       << setfill(' ') << dec << endl;

  cout << setw(25) << "Program Header Offset" << " : "
       << get_program_header_table_offset() << endl;

  cout << setw(25) << "Section Header Offset" << " : "
       << get_section_header_table_offset() << endl;

  cout << setw(25) << "Flags" << " : " << get_flags() << endl;

  cout << setw(25) << "ELF Header Size" << " : "
       << get_elf_header_size() << endl;

  cout << setw(25) << "Program Header Size" << " : "
       << get_program_header_entry_size() << endl;

  cout << setw(25) << "Program Header Num" << " : "
       << get_program_header_num() << endl;

  cout << setw(25) << "Section Header Size" << " : "
       << get_section_header_entry_size() << endl;

  cout << setw(25) << "Section Header Num" << " : "
       << get_section_header_num() << endl;

  cout << setw(25) << "String Section Index" << " : "
       << get_str_section_index() << endl;

  cout << setw(79) << setfill('=') << '=' << endl << endl;
}

char const *elf_header::get_class_name(int clazz) {
  switch (clazz) {
  default:
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFCLASSNONE, "Invalid class")
  CASE_PAIR(ELFCLASS32, "32bit")
  CASE_PAIR(ELFCLASS64, "64bit")
#undef CASE_PAIR
  }
}

char const *elf_header::get_endianness_name(int endianness) {
  switch (endianness) {
  default:
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFDATANONE, "Invalid endianness")
  CASE_PAIR(ELFDATA2LSB, "Little endian")
  CASE_PAIR(ELFDATA2MSB, "Big endian")
#undef CASE_PAIR
  }
}

char const *elf_header::get_os_abi_name(int abi) {
  if (abi >= 64 && abi <= 255) {
    return "Architecture specific";
  }

  switch (abi) {
  default: return "Unknown OS ABI";
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFOSABI_NONE, "No extensions or not specified")
  CASE_PAIR(ELFOSABI_HPUX, "HP-UX")
  CASE_PAIR(ELFOSABI_NETBSD, "NetBSD")
  CASE_PAIR(ELFOSABI_LINUX, "Linux")
  CASE_PAIR(ELFOSABI_SOLARIS, "Solaris")
  CASE_PAIR(ELFOSABI_AIX, "AIX")
  CASE_PAIR(ELFOSABI_FREEBSD, "FreeBSD")
  CASE_PAIR(ELFOSABI_TRU64, "Tru64")
  CASE_PAIR(ELFOSABI_MODESTO, "Modesto")
  CASE_PAIR(ELFOSABI_OPENBSD, "OpenBSD")
#undef CASE_PAIR
  }
}

char const *elf_header::get_object_type_name(uint16_t type) {
  switch (type) {
  default: return "No file type";

  case ET_REL:  return "Relocatable file";
  case ET_EXEC: return "Executable file";
  case ET_DYN:  return "Shared object file";
  case ET_CORE: return "Core file";

  case ET_LOOS: case ET_HIOS:
    return "Operating system-specific";

  case ET_LOPROC: case ET_HIPROC:
    return "Processor-specific";
  }
}

char const *elf_header::get_machine_name(uint16_t machine) {
  switch (machine) {
    default: return "No machine or unknown";
    case EM_386: return "Intel 80386 (X86)";
    case EM_ARM: return "Advanced RISC Machine (ARM)";
  }
}

char const *elf_header::get_version_name(uint32_t version) {
  switch (version) {
    default: return "Invalid version";
    case EV_CURRENT: return "Current version";
  }
}

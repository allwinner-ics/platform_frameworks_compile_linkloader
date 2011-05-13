#ifndef ELF_SYM_ENTRY_H
#define ELF_SYM_ENTRY_H

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <string>

#include <elf.h>

class elf_object;

class elf_sym_entry {
private:
  elf_object const &owner;

protected:
  elf_sym_entry(elf_object const &obj) : owner(obj) {
  }

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_sym_entry> read(Archiver &AR,
                                                    elf_object const &);

  void print() const;

  template <typename Archiver>
  void serialize(Archiver &AR);

  char const *get_name() const;
  virtual uint32_t get_name_index() const = 0;
  virtual uint8_t get_type() const = 0;
  virtual uint8_t get_binding_attribute() const = 0;
  virtual uint8_t get_visibility() const = 0;
  virtual uint16_t get_section_index() const = 0;
  virtual uint64_t get_value() const = 0;
  virtual uint64_t get_size() const = 0;

private:
  // Read ELF header
  template <typename Archiver>
    static boost::shared_ptr<elf_sym_entry> read_32(Archiver &AR,
                                                    elf_object const &);

  template <typename Archiver>
    static boost::shared_ptr<elf_sym_entry> read_64(Archiver &AR,
                                                    elf_object const &);

  static char const *get_type_name(uint8_t);
  static char const *get_binding_attribute_name(uint8_t);
  static char const *get_visibility_name(uint8_t);

};

extern template boost::shared_ptr<elf_sym_entry>
elf_sym_entry::read(serialization::archive_reader_le &,
                    elf_object const &);

extern template boost::shared_ptr<elf_sym_entry>
elf_sym_entry::read(serialization::archive_reader_be &,
                    elf_object const &);


#endif // ELF_SYM_ENTRY_H

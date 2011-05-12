#ifndef ELF_READER_H
#define ELF_READER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#include <stdio.h>

class elf_ident;
class elf_header;
class elf_section_header;
class elf_program_header;
class elf_section;
class elf_strtab;
class elf_symtab;

class elf_object {
private:
  static const std::string SYMBOL_TABLE_NAME;
  static const std::string SYMBOL_STR_TAB_NAME;

  boost::shared_ptr<elf_header> header;
  boost::shared_ptr<elf_strtab> section_header_str_tab;
  boost::shared_ptr<elf_symtab> symbol_table;
  boost::shared_ptr<elf_strtab> symbol_str_tab;

  std::vector<boost::shared_ptr<elf_section_header> > sh_table;
  std::vector<boost::shared_ptr<elf_section> > s_table;
  std::vector<boost::shared_ptr<elf_program_header> > ph_table;

protected:
  elf_object() {
  }

public:
  static boost::shared_ptr<elf_object> read(std::string const &filename);

  elf_header const &get_header() const { return *header; }

  elf_section_header const &get_section_header(size_t index) const {
    return *sh_table[index];
  }

  elf_section_header const &get_section_header(const std::string &str) const;

  elf_section const &get_section(size_t index) const {
    return *s_table[index];
  }

  elf_strtab const &get_section_header_str_tab() const {
    return *section_header_str_tab;
  }

  elf_strtab const &get_symbol_str_tab() const {
    return *symbol_str_tab;
  }

  void print() const;

private:
  template <typename Archiver>
  inline void read_header(Archiver &AR);

  template <typename Archiver>
  inline void read_section_header_table(Archiver &AR);

  template <typename Archiver>
  inline void read_section_header_str_tab(Archiver &AR);

  template <typename Archiver>
  inline void read_symbol_table(Archiver &AR);

  template <typename Archiver>
  inline void read_symbol_str_tab(Archiver &AR);

  template <typename archiver>
  void read_internal(archiver &ar);

  // FIXME: Remove this function in the future.
  static bool open_map_file(std::string const &filename,
                            int &fd,
                            unsigned char const *&image,
                            size_t &size);

  // FIXME: Remove this function in the future.
  static void close_map_file(int fd, unsigned char const *ptr, size_t size);

};

#endif // ELF_READER_H

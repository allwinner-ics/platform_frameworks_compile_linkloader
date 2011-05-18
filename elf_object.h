#ifndef ELF_READER_H
#define ELF_READER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#include <stdio.h>

#include "ELFHeader.h"

class elf_object {
private:
  bool is64bit;

  boost::shared_ptr<ELFHeader<32> > header32;
  boost::shared_ptr<ELFHeader<64> > header64;

protected:
  elf_object() {
  }

public:
  static boost::shared_ptr<elf_object> read(std::string const &filename);

  void print() const;

private:
  template <typename Archiver>
  inline void read_header(Archiver &AR);

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

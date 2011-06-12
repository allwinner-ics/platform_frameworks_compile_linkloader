#ifndef MEM_CHUNK_H
#define MEM_CHUNK_H

#include <stddef.h>

class MemChunk {
private:
  unsigned char *buf;
  size_t buf_size;

public:
  MemChunk();

  ~MemChunk();

  bool allocate(size_t size);

  void print() const;

  bool protect(int prot);

  unsigned char const *getBuffer() const {
    return buf;
  }

  unsigned char *getBuffer() {
    return buf;
  }

  unsigned char &operator[](size_t index) {
    return buf[index];
  }

  unsigned char const &operator[](size_t index) const {
    return buf[index];
  }

  size_t size() const {
    return buf_size;
  }

};

#include "impl/MemChunk.hxx"

#endif // MEM_CHUNK_H

#ifndef RAW_OSTREAM_H
#define RAW_OSTREAM_H

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

extern llvm::raw_ostream &out();
struct MyFormat {
  char *ptr;
};
extern MyFormat const fillformat(char const,        // Fill character.
                                 int const,         // Fill Width.
                                 char const * = "", // Format string.
                                 ...);              // Format variable.
extern llvm::raw_ostream &operator<<(llvm::raw_ostream &, MyFormat const &);

#endif // RAW_OSTREAM_H

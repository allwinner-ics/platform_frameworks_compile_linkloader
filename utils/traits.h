#ifndef TRAITS_H
#define TRAITS_H

#include <stddef.h>

template <typename Type>
struct TypeTraits {
private:
  struct AlignmentTest {
    char pending;
    Type element;
  };

public:
  enum { size = sizeof(Type) };
  enum { align = offsetof(AlignmentTest, element) };
};

#endif // TRAITS_H

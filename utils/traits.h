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

#define TYPE_TRAITS_SPECIALIZE(TYPE, SIZE, ALIGN) \
template <> \
struct TypeTraits<TYPE> { \
  enum { size = SIZE }; \
  enum { align = ALIGN }; \
};

#endif // TRAITS_H

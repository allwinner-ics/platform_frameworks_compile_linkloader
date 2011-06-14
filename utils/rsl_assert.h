#ifndef RSL_ASSERT_H
#define RSL_ASSERT_H

#include <llvm/Support/raw_ostream.h>

#define ASSERT_FAIL(mes)                                      \
  ((llvm::errs() << "RSL ASSERT FAIL!\n"                      \
  << __FILE__ << ':' << __LINE__ << ": "                      \
  << "rsl_assert(" << (mes) << ")\n"), abort())

#ifdef RSL_NDEBUG

#define rsl_assert(con) (void)(0)

#else

#define rsl_assert(con) \
  ((con)                \
  ? (void)(0)           \
  : ASSERT_FAIL(#con))


#endif

#endif // RSL_ASSERT_H

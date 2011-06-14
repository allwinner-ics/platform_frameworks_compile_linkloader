#ifndef RSL_ASSERT_H
#define RSL_ASSERT_H

#if defined(__cplusplus)
extern "C" {
#endif

extern void ASSERT_FAILED(char const *file,
                          unsigned line,
                          char const *expr);

#if defined(__cplusplus)
} // extern "C"
#endif

#ifdef RSL_NDEBUG

#define rsl_assert(EXPR) \
  do { } while (0)

#else

#define rsl_assert(EXPR)                                      \
  do {                                                        \
    if (!(EXPR)) {                                            \
      ASSERT_FAILED(__FILE__, __LINE__, #EXPR);               \
    }                                                         \
  } while (0)

#endif

#endif // RSL_ASSERT_H

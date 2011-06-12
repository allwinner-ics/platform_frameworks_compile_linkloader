#ifndef LIBRSLOADER_H
#define LIBRSLOADER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RSExecOpaque;
typedef struct RSExecOpaque *RSExecRef;

RSExecRef rsloaderCreateExec(unsigned char const *buf,
                             size_t buf_size,
                             void *(*find_symbol)(void *, char const *),
                             void *find_symbol_context);

void rsloaderDisposeExec(RSExecRef object);

void *rsloaderGetSymbolAddress(RSExecRef object, char const *name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRSLOADER_H */

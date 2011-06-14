#include "librsloader.h"
#include "utils/rsl_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct func_entry_t {
  char const *name;
  size_t name_len;
  void *addr;
};

void *find_sym(void *context, char const *name) {
  static struct func_entry_t const tab[] = {
#define DEF(NAME, ADDR) \
    { NAME, sizeof(NAME) - 1, (void *)(&(ADDR)) },

    DEF("printf", printf)
    DEF("scanf", scanf)
    DEF("__isoc99_scanf", scanf)
    DEF("rand", rand)
    DEF("time", time)
    DEF("srand", srand)
#undef DEF
  };

  static size_t const tab_size = sizeof(tab) / sizeof(struct func_entry_t);

  // Note: Since our table is small, we are using trivial O(n) searching
  // function.  For bigger table, it will be better to use binary
  // search or hash function.
  size_t i;
  size_t name_len = strlen(name);
  for (i = 0; i < tab_size; ++i) {
    if (name_len == tab[i].name_len && strcmp(name, tab[i].name) == 0) {
      return tab[i].addr;
    }
  }

  rsl_assert(0 && "Can't find symbol.");
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "USAGE: %s [ELF] [ARGS]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "ERROR: Unable to open the file: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  struct stat sb;
  if (fstat(fd, &sb) != 0) {
    fprintf(stderr, "ERROR: Unable to stat the file: %s\n", argv[1]);
    close(fd);
    exit(EXIT_FAILURE);
  }

  unsigned char const *image = (unsigned char const *)
    mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (image == MAP_FAILED) {
    fprintf(stderr, "ERROR: Unable to mmap the file: %s\n", argv[1]);
    close(fd);
    exit(EXIT_FAILURE);
  }

  RSExecRef object = rsloaderCreateExec(image, sb.st_size, find_sym, 0);
  if (!object) {
    fprintf(stderr, "ERROR: Unable to load elf object.\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  int (*main_stub)(int, char **) =
    (int (*)(int, char **))rsloaderGetSymbolAddress(object, "main");

  int ret = main_stub(argc - 1, argv + 1);
  printf("============================================================\n");
  printf("ELF object finished with code: %d\n", ret);
  fflush(stdout);

  rsloaderDisposeExec(object);

  close(fd);

  return EXIT_SUCCESS;
}

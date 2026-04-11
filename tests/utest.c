#define _POSIX_C_SOURCE 200809L

#include "utest.h"

static int is_verbose_flag(const char *arg) {
  return !strcmp(arg, "-v") || !strcmp(arg, "--verbose");
}

static int is_mode_flag(const char *arg) {
  return !strcmp(arg, "-d") || !strcmp(arg, "-m");
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4)
    errx(1, "Usage: %s [-v|--verbose] [-d|-m] <file|directory>\n", argv[0]);

  int argi = 1;
  int verbose = 0;
  int interpret = 1;

  if (argi < argc && is_verbose_flag(argv[argi])) {
    verbose = 1;
    ++argi;
  }

  if (argi < argc && is_mode_flag(argv[argi])) {
    interpret = strcmp(argv[argi], "-d") != 0;
    ++argi;
  }

  if (argi != argc - 1)
    errx(1, "Usage: %s [-v|--verbose] [-d|-m] <file|directory>\n", argv[0]);

  // Check if the input is a directory
  DIR *dir = opendir(argv[argi]);
  if (dir != NULL) {
    closedir(dir);
    run_compare_directory(argv[argi], verbose, interpret);
  } else {
    // Assume the input is a single file
    run_compare_file(argv[argi], verbose, interpret);
  }

  return 0;
}

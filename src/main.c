#include "main.h"

int main(int argc, char **argv) {
  if (argc < 2)
    errx(1, "Usage: %s [-d|-m] <file>", argv[0]);

  if (argv[1][0] == '-' && strcmp(argv[1], "-d") && strcmp(argv[1], "-m"))
    errx(1, "Invalid flag: %s", argv[1]);

  // Check the flag & process the file accordingly
  if (!strcmp(argv[1], "-d")) {
    // Exit of no file to dump provided
    if (argc < 3)
      errx(1, "File to dump not provided.\nUsage: %s -d <file>", argv[0]);

    // Set the flag to dump
    DISASSEMBLE = 1;
  } else if (!strcmp(argv[1], "-m")) {
    // Exit of no file to interpret provided
    if (argc == 2)
      errx(1, "File to interpret not provided.\nUsage: %s -m <file>", argv[0]);

    // Set the flag to debug-interpret
    DEBUG = 1;
    INTERPRET = 1;
  } else {
    INTERPRET = 1; // No flag => INTERPRET
  }

  // Treat spaces in the path
  __treat_spc(argc, argv);

  // Dump the file in text & data memories
  char *path = argv[argc - 1];
  hexdump(path); // Contains error for unexisting file

  if (INTERPRET || DEBUG) { // more readable
    // Setup the memory for the interpreter
    setup_memory(argc, argv);

    if (DEBUG)
      print_regs_header(); // Interpret the binary
  }

  // Disassemble the binary (doesn't interpret if only -d)
  disassemble();

  if (DISASSEMBLE) {
    // Print the ASM code to STDERR (optimized printing)
    for (size_t i = 0; i <= ASM_MAX_INDEX; i++) {
      if (strcmp(ASM[i], ""))
        fprintf(stderr, "%s\n", ASM[i]);
    }
    return EXIT_SUCCESS;
  }

  return EXIT_SUCCESS;
}
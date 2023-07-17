#include "main.h"

int main(int argc, char **argv) {
  // Check the flag & process the file accordingly
  if (!strcmp(argv[1], "-d")) {
    // Exit of no file to dump provided
    if (argc == 2)
      errx(1, "File to dump not provided.\nUsage: %s -d <file>", argv[0]);

    // Set the flag to dump
    DISASSEMBLE = 1;
  } else if (!strcmp(argv[1], "-m")) {
    // Exit of no file to interpret provided
    if (argc == 2)
      errx(1, "File to interpret not provided.\nUsage: %s -m <file>", argv[0]);

    // Set the flag to debug-interpret
    DEBUG = INTERPRET = 1;
  } else {
    INTERPRET = 1; // No flag => INTERPRET
  }

  // Dump the file in text & data memories
  char *path = argv[argc - 1];
  hexdump(path); // Contains error for unexisting file

  if (DEBUG) { // => interpret as well
    // Setup the memory for the interpreter
    setup_memory(argc, argv);

    if (DEBUG) { // Print only if -m
      // Interpret the binary
      print_regs_header();
    }
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
}
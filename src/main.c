#include "main.h"

int main(int argc, char **argv) {
  // Check the number of arguments
  if (argc < 3)
    errx(1, "Wrong number of arguments.\nUsage: %s <flags> <file>", argv[0]);

  // Dump the file in both memories
  char *path = argv[2];
  hexdump(path);

  // Check the flag & process the file accordingly
  if (!strcmp(argv[1], "-d")) {
    // Exit of no file to dump provided
    if (argc == 2)
      errx(1, "File to dump not provided.\nUsage: %s -d <file>", argv[0]);

    // Disassemble the binary
    disassemble();

    // Print the ASM code to STDERR (optimized printing)
    for (size_t i = 0; i <= ASM_MAX_INDEX; i++) {
      if (strcmp(ASM[i], ""))
        fprintf(stderr, "%s\n", ASM[i]);
    }
  } else if (!strcmp(argv[1], "-m")) { // INTERPRET
    // Exit of no file to interpret provided
    if (argc == 2)
      errx(1, "File to interpret not provided.\nUsage: %s -m <file>", argv[0]);

    // Set the flag to interpret
    INTERPRET = 1;

    // Setup the memory for the interpreter
    setup_memory(argc, argv);

    // Interpret the binary
    print_regs_header();

    // Disassemble & interpret the binary
    disassemble();

    // while (1) {
    //   NodeAST node;
    //   NodeAST_init(&node);
    //   get_node(&node, ASM[IP]);
    //   int ret = interpret(&node);

    //   // Exit if the instruction was an exit syscall
    //   if (ret == EXIT_SYSCALL)
    //     break;

    //   // Only increment the IP if the instruction was not a jump
    //   if (ret != EXIT_IPCHANGED)
    //     IP += node.opLen;
    // }

    // DONT FORGET TO ACTUALLY EXECUTE IT AT THE END
    // TODO, do like ASM global array to print to stderr when done
  } else {
    // Raise an error if the flag is unknown
    errx(1, "Unknown flag: %s\nUsage: %s <flags> <file>", argv[1], argv[0]);
  }

  // Free & exit
  return 0;
}
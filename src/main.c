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

    // ONLY DISASSEMBLER
    // TODO
  } else if (!strcmp(argv[1], "-m")) { // INTERPRET
    // Exit of no file to interpret provided
    if (argc == 2)
      errx(1, "File to interpret not provided.\nUsage: %s -m <file>", argv[0]);

    // Disassemble the binary
    translate_bin();

    // Setup the memory for the interpreter
    Vector args;
    vector_init(&args);
    for (size_t i = 2; i < argc; ++i)
      vector_pushback(&args, argv[i]);
    Vector envp = get_envp();
    process_args(&args, &envp);
    // print_hexdump(data_mem, MEMORY_SIZE);

    // Print the ASM code
    // for (size_t i = 0; i < 1000; i++) {
    //   if (strcmp(ASM[i], ""))
    //     printf("%lx: %s", i, ASM[i]);
    //   else
    //     printf("%lx: \n", i);
    // }

    // Lex & parse the assembly code
    size_t lenAST = 0;
    NodeAST **AST = malloc(MAX_INSTR * sizeof(NodeAST *));
    build_AST(ASM, MEMORY_SIZE, &AST, &lenAST);

    // Interpret each node of the AST
    size_t i = 0;
    print_regs_header();
    while (i < lenAST && !interpret(AST[i++]))
      ;

    // Free the AST & the ASM code
    free_AST(AST, lenAST);
    // free_2d(ASM, MEMORY_SIZE);
  } else {
    // Raise an error if the flag is unknown
    errx(1, "Unknown flag: %s\nUsage: %s <flags> <file>", argv[1], argv[0]);
  }

  // Free & exit
  return 0;
}
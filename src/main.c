#include "main.h"

int main(int argc, char **argv) {
  // Check the number of arguments
  if (argc < 3)
    errx(1, "Wrong number of arguments.\nUsage: %s <flags> <file>", argv[0]);

  // Patch unknown flag error + unknown flag & still compiling

  // Initialize the global buffer
  // TODO: CLEAN BUFFER TO REDUCE NUMBER OF ALLOCATIONS
  unsigned char *buffer = malloc(BUFFER_SIZE * sizeof(unsigned char));
  if (buffer == NULL)
    errx(1, "Can't allocate memory for the buffer!");

  // Dump the file & get the content in the buffer & text/data area sizes
  size_t buffer_size;
  hexdump(argv[2], buffer, &buffer_size, &TEXT_SIZE, &DATA_SIZE);

  // Update the global variables
  TEXT_BEG = HEADER_SIZE;
  DATA_BEG = HEADER_SIZE + TEXT_SIZE;

  // Fill the memories with the buffer content
  memcpy(text_mem, buffer + HEADER_SIZE, TEXT_SIZE);
  memcpy(data_mem, buffer + DATA_BEG, DATA_SIZE);
  // TODO fix that ^

  // Check the flag & process the file accordingly
  if (!strcmp(argv[1], "-d")) { // DUMP
    // Exit of no file to dump provided
    if (argc == 2)
      errx(1, "File to dump not provided.\nUsage: %s -d <file>", argv[0]);

    // Print the hexadecimal dump
    print_hexdump(buffer, buffer_size);
    printf("Text-area size: 0x%08lx = %zu bytes\n", TEXT_SIZE, TEXT_SIZE);
    printf("Data size: 0x%08lx = %zu bytes\n", DATA_SIZE, DATA_SIZE);
  } else if (!strcmp(argv[1], "-m")) { // INTERPRET
    // Exit of no file to interpret provided
    if (argc == 2)
      errx(1, "File to interpret not provided.\nUsage: %s -m <file>", argv[0]);

    // Call the disassembler executable
    char command[COMMAND_SIZE];
    snprintf(command, COMMAND_SIZE, "./disassembler %s", argv[2]);
    size_t len = 0;
    char **ASM_code = get_cout(command, &len);
    // TODO: merge DISASSEMBLING & INTERPRETING !!

    // Setup the memory for the interpreter
    Vector args;
    vector_init(&args);
    for (size_t i = 2; i < argc; ++i)
      vector_pushback(&args, argv[i]);
    Vector envp = get_envp();
    process_args(&args, &envp);
    // print_hexdump(data_mem, MEMORY_SIZE);

    // Lex & parse the assembly code
    size_t lenAST = 0;
    NodeAST **AST = malloc(MAX_INSTR * sizeof(NodeAST *));
    build_AST(ASM_code, len, &AST, &lenAST);

    // Interpret each node of the AST
    size_t i = 0;
    print_regs_header();
    while (i < lenAST && !interpret(AST[i++]))
      ;

    // Free the AST & the ASM code
    free_AST(AST, lenAST);
    free_2d(ASM_code, len);
  } else {
    // Raise an error if the flag is unknown
    errx(1, "Unknown flag: %s\nUsage: %s <flags> <file>", argv[1], argv[0]);
  }

  // Free & exit
  free(buffer);
  return 0;
}
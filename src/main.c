#include "main.h"

int main(int argc, char **argv) {
  // Check the number of arguments
  if (argc < 2 || argc > 3)
    errx(1, "Usage: %s <flags> <file>", argv[0]);

  // Check the flags
  if (argc == 2 && (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--dump")))
    errx(1, "Usage: %s -d <file>", argv[0]);

  if (argc == 3 && argv[1][0] != '-')
    errx(1, "Provided flag not properly formatted: %s", argv[1]);

  if (argc == 3 && strcmp(argv[1], "-d") && strcmp(argv[1], "--dump"))
    errx(1, "Unknown flag: %s", argv[1]);

  // Initialize the global buffer
  unsigned char *buffer = malloc(BUFFER_SIZE * sizeof(unsigned char));
  if (buffer == NULL)
    errx(1, "Can't allocate memory for the buffer!");

  // Dump the file & get the content in the buffer & text/data area sizes
  size_t buffer_size;
  hexdump(argv[argc - 1], buffer, &buffer_size, &TEXT_SIZE, &DATA_SIZE);

  // Update the global variables
  TEXT_BEG = HEADER_SIZE;
  TEXT_END = TEXT_BEG + TEXT_SIZE;
  DATA_BEG = HEADER_SIZE + TEXT_SIZE;
  DATA_END = DATA_BEG + DATA_SIZE;

  // Initialize the text buffer
  unsigned char *text = malloc(TEXT_SIZE * 2 * sizeof(unsigned char));
  if (text == NULL)
    errx(1, "Can't allocate memory for the text area!");

  // Initialize the data buffer
  unsigned char *data = malloc(DATA_SIZE * 2 * sizeof(unsigned char));
  if (data == NULL)
    errx(1, "Can't allocate memory for the data area!");

  // Copy the text & data area to their respective buffers
  memcpy(text, buffer + HEADER_SIZE, TEXT_SIZE);
  memcpy(data, buffer + HEADER_SIZE + TEXT_SIZE, DATA_SIZE);

  // Print according to the number of arguments
  if (argc == 2) {
    size_t i = 0;

    // Call the disassembler executable
    char command[COMMAND_SIZE];
    snprintf(command, COMMAND_SIZE, "./disassembler %s", argv[argc - 1]);

    // TODO: Will need to be changed when merging disassembly & interpreter
    size_t len = 0;
    char **ASM_code = get_cout(command, &len);

    // Lex & parse the assembly code
    size_t lenAST = 0;
    NodeAST **AST = malloc(MAX_INSTR * sizeof(NodeAST *));
    build_AST(ASM_code, len, &AST, &lenAST);

    // Interpret each node of the AST
    print_regs_header();
    while (i < lenAST && !interpret(AST[i++], text, data))
      ;

    // Free the AST & the ASM code
    free_AST(AST, lenAST);
    free_2d(ASM_code, len);
  } else {
    // Print the hexadecimal dump & the text area
    print_hexdump(buffer, buffer_size);
    printf("Text-area size: 0x%08lx = %zu bytes\n", TEXT_SIZE, TEXT_SIZE);
    printf("Data size: 0x%08lx = %zu bytes\n", DATA_SIZE, DATA_SIZE);
  }

  // Free & exit
  free(buffer);
  free(text);
  free(data);
  return 0;
}
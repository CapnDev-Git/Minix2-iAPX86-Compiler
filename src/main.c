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
  size_t buffer_size, program_size, data_size;
  hexdump(argv[argc - 1], buffer, &buffer_size, &program_size, &data_size);

  // Initialize the program buffer
  unsigned char *program = malloc(program_size * 2 * sizeof(unsigned char));
  if (program == NULL)
    errx(1, "Can't allocate memory for the text area!");

  // Initialize the data buffer
  unsigned char *data = malloc(data_size * 2 * sizeof(unsigned char));
  if (data == NULL)
    errx(1, "Can't allocate memory for the data area!");

  // Copy the text & data area to their respective buffers
  memcpy(program, buffer + HEADER_SIZE, program_size);
  memcpy(data, buffer + HEADER_SIZE + program_size, data_size);

  // Print according to the number of arguments
  if (argc == 2) {
    // Call the disassembler executable
    char command[COMMAND_SIZE];
    snprintf(command, COMMAND_SIZE, "./disassembler %s", argv[argc - 1]);

    // TODO: Will need to be changed when merging disassembly & interpreter
    size_t lc = 0;
    char **ASM_code = get_cout(command, &lc);
    // for (size_t i = 0; i < lc; i++)
    // printf("%s\n", ASM_code[i]);

    lex(ASM_code, lc);
    // parse(ASM_code, lc);
    // interpret(ASM_code, lc);
  } else {
    // Print the hexadecimal dump & the text area
    print_hexdump(buffer, buffer_size);
    printf("Text-area size: 0x%08lx = %zu bytes\n", program_size, program_size);
    printf("Data size: 0x%08lx = %zu bytes\n", data_size, data_size);
  }

  // Free & exit
  free(buffer);
  free(program);
  free(data);
  return 0;
}
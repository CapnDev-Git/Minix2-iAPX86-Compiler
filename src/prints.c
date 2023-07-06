#include "prints.h"

void print_hexdump(const unsigned char *buffer, size_t buffer_size) {
  // Print the hexadecimal dump
  size_t i, j;
  for (i = 0; i < buffer_size; i += HEX_PER_LINE) {
    printf(ADDRESS_FORMAT, i);

    // Print the hexadecimal representation
    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      if (j % (HEX_PER_LINE / 2) == 0 && j != 0)
        printf(SPACE);
      printf(HEX_FORMAT_L, (unsigned char)buffer[i + j]);
    }

    // Pad the last line with spaces
    if (j < HEX_PER_LINE) {
      size_t padding_size = (HEX_PER_LINE - j) * 3 + 1;
      printf("%*s", (int)padding_size, "");
    }

    // Print the ASCII representation
    printf(SPACE ASCII_SEPARATOR);
    for (j = 0; j < HEX_PER_LINE && (i + j) < buffer_size; j++) {
      unsigned char c = buffer[i + j];
      printf("%c", isprint(c) ? c : NO_CHAR);
    }

    printf(ASCII_SEPARATOR "\n");
  }

  printf(ADDRESS_FORMAT "\n\n", i - HEX_PER_LINE + j);
}

void print_regs_header() {
  printf(" AX   BX   CX   DX   SP   BP   SI   DI  FLAGS IP\n");
}

void print_regs_status(uint16_t *r, char *f, char *l, char *m) {
  printf(REGS_STATUS, r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
  printf(FLAGS_STATUS, f[0], f[1], f[2], f[3]);
  printf(LINE_STATUS, l);

  // Print the memory content if necessary
  if (m != NULL)
    printf(MEMORY_CONTENT, m);
  else
    printf("\n");
}

void printOK(const char *filename) {
  printf("\033[1;32mOK\033[0m %s\n", filename);
}

void printKO(const char *filename) {
  printf("\033[1;31mKO\033[0m %s\n", filename);
}

void printDiff(const char *errorLine, const char *ref) {
  const char *lineStart = errorLine;
  const char *lineEnd = errorLine;
  while (lineStart > errorLine - 50 && *lineStart != '\n')
    --lineStart;
  while (*lineEnd != '\0' && *lineEnd != '\n')
    ++lineEnd;

  printf("\033[0m");
  for (const char *ch = lineStart; ch < errorLine; ++ch)
    putchar(*ch);

  printf("\033[1;31m");
  putchar(*errorLine);

  for (const char *ch = errorLine + 1; ch < lineEnd; ++ch)
    putchar(*ch);
  printf("\033[0m");

  lineStart = ref;
  lineEnd = ref;
  while (lineStart > ref - 50 && *lineStart != '\n')
    --lineStart;
  while (*lineEnd != '\0' && *lineEnd != '\n')
    ++lineEnd;

  printf("\033[0m");
  for (const char *ch = lineStart; ch < ref; ++ch) {
    putchar(*ch);
  }

  printf("\033[1;32m");
  putchar(*ref);
  for (const char *ch = ref + 1; ch < lineEnd; ++ch) {
    putchar(*ch);
  }

  printf("\033[0m\n");
}

void print4b(const unsigned char *p, size_t a, size_t il, size_t *ip,
             char *line) {
  // Print the address
  sprintf(line, DIS_ADDRESS_FORMAT, a);
  for (size_t j = 0; j < il; j++) {
    sprintf(line + strlen(line), INSTRUCTION_FORMAT, p[a + j]);
  }

  // Print the padding
  size_t padding_size = 14 - 2 * il;
  for (size_t j = 0; j < padding_size; j++)
    sprintf(line + strlen(line), SPACE);

  // Increment the instruction pointer by the instruction length
  *ip += il;
}
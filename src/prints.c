#include "prints.h"

void print_hexdump(const unsigned char *buffer, size_t buffer_size) {
  // Print the headers
  // print_headers(HEX_FORMAT_U, 2, HEX_PER_LINE);

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

void print_regs_status(size_t *r, char *f, char *l) {
  printf(REGS_STATUS, r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
  printf(FLAGS_STATUS, f[0], f[1], f[2], f[3]);
  printf(LINE_STATUS, l);
}
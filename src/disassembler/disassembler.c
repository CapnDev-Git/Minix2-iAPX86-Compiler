#include "disassembler.h"

void get_adm(size_t a, unsigned char mod, unsigned char rm, char *ea,
             size_t *ds) {
  size_t disp = 0;
  unsigned char adisp;

  // Patch the special case of mod = 0b00 and rm = 0b110
  if (mod == 0b00 && rm == 0b110) {
    *ds = 2;
    disp = text_mem[a] + (text_mem[a + 1] << 8);
    sprintf(ea, "[%04lx]", disp);
    return;
  }

  // Get the destination
  char *DESTS[DESTS_SIZE] = {"bx+si", "bx+di", "bp+si", "bp+di",
                             "si",    "di",    "bp",    "bx"};
  char *dest = DESTS[rm];

  // Format the effective address according to the addressing mode &
  // destination + DISP size & value
  switch (mod) {
  case 0b01:
    *ds = 1;
    disp = text_mem[a];
    adisp = (disp & 0x80) ? (~disp + 1) : disp;
    sprintf(ea, "[%s%c%x]", dest, (disp & 0x80) ? '-' : '+', adisp);
    break;

  case 0b10:
    *ds = 2;
    disp =
        (int16_t)((text_mem[a + 1] << 8) | text_mem[a]); // disp-high, disp-low
    int16_t adisp16 = (disp & 0x8000) ? (~disp + 1) : disp;
    sprintf(ea, "[%s%c%x]", dest, (disp & 0x8000) ? '-' : '+', adisp16);
    break;

  case 0b00:
  case 0b11:
    *ds = 0;
    sprintf(ea, !mod ? "[%s]" : "%s", dest);
    break;
  }
}

void disassemble() {
  // Initialize the segment registers (for segment override prefix)
  char line[256];
  unsigned char op;
  size_t a = 0;

  // Array of function pointers to the opcodes
  void (*opcodes[OPCODES_SIZE])(size_t, unsigned char) = {
      f0x0, f0x1, f0x2, f0x3, f0x4, f0x5, f0x6, f0x7,
      f0x8, f0x9, f0xa, f0xb, f0xc, f0xd, f0xe, f0xf};

  // Keep looping until the end of the program
  while (a < TEXT_SIZE) {
    // Get the opcode
    op = text_mem[a] >> 4;

    // Check if there's an undefined opcode (end of program)
    if (!op) {
      if (a + 1 >= TEXT_SIZE) {
        print4b(text_mem, a, 1, line);
        sprintf(line + strlen(line), "(undefined)");

        // Add the line to the lsit of ASM ligns
        strncpy(ASM[a], line, sizeof(ASM[a]));
        ASM_MAX_INDEX = a;
        a = ++IP;
        return;
      }
    }

    // Catch segment override prefix
    if ((op >> 1) == 0b000 && ((text_mem[a] >> 1) & 0b11) == 0b11) {
      print4b(text_mem, a, 1, line);

      if (text_mem[a] == 0x0f) {
        sprintf(line + strlen(line), "(undefined)");
      } else {
        if (text_mem[a] & 0b1)
          sprintf(line + strlen(line), "pop %s",
                  SEGREG[(text_mem[a] >> 3) & 0b11]);
        else
          sprintf(line + strlen(line), "push %s",
                  SEGREG[(text_mem[a] >> 3) & 0b11]);
      }

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;
      a = ++IP;
      continue;
    }

    // Execute the corresponding opcode function
    opcodes[op](a, text_mem[a] & 0b1111);

    // Interpret the disassembled instruction if the flag is set
    if (INTERPRET) {
      // Define the node
      NodeAST node;
      NodeAST_init(&node);

      // Parse & interpret the node
      get_node(&node, ASM[a]);
      int ret = interpret(&node);

      // Exit if the instruction was an exit syscall
      if (ret == EXIT_SYSCALL) {
        printf("Program exited");
        return;
      }
    }

    // Update the address to go to IP (changed by opcode function / interpreter)
    a = IP;
  }
}
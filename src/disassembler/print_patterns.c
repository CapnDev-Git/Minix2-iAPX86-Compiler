#include "print_patterns.h"
void printsw(size_t a, char *instr, char *ea, unsigned char LSB4, size_t *ds,
             NodeAST *node) {
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  get_adm(a + 2, mod, rm, ea, ds);

  char line[256]; // Buffer to store the formatted string
  size_t il = 0;

  switch (LSB4 & 0b11) {
  case 0b00:
    print4b(text_mem, a, 2 + *ds + 1, line);
    sprintf(line + strlen(line), "%s %s%s, %x", instr, mod != 3 ? "byte " : "",
            mod == 3 ? REG8[rm] : ea, text_mem[a + 2 + *ds]);
    NodeAST_set_opcode(node, instr);
    if (mod == 3)
      NodeAST_add_reg_operand(node, mod == 3 ? REG8[rm] : ea,
                              mod == 3 ? rm + 8 : rm);
    else
      NodeAST_add_mem_operand(node, ea, "byte");
    {
      char imm[16];
      sprintf(imm, "%x", text_mem[a + 2 + *ds]);
      NodeAST_add_imm_operand(node, imm, text_mem[a + 2 + *ds]);
    }
    il = 2 + *ds + 1;
    break;

  case 0b01:
    print4b(text_mem, a, 2 + *ds + 2, line);
    sprintf(line + strlen(line), "%s %s, %04x", instr,
            mod == 3 ? REG16[rm] : ea, *(uint16_t *)&text_mem[a + 2 + *ds]);
    NodeAST_set_opcode(node, instr);
    if (mod == 3)
      NodeAST_add_reg_operand(node, REG16[rm], rm);
    else
      NodeAST_add_mem_operand(node, ea, NULL);
    {
      char imm[16];
      sprintf(imm, "%04x", *(uint16_t *)&text_mem[a + 2 + *ds]);
      NodeAST_add_imm_operand(node, imm, *(uint16_t *)&text_mem[a + 2 + *ds]);
    }
    il = 2 + *ds + 2;
    break;

  case 0b10:
    print4b(text_mem, a, 2 + *ds, line);
    sprintf(line + strlen(line), "%s %s, %02x", instr, mod == 3 ? REG8[rm] : ea,
            *(uint8_t *)&text_mem[a + 2]);
    NodeAST_set_opcode(node, instr);
    if (mod == 3)
      NodeAST_add_reg_operand(node, REG8[rm], rm + 8);
    else
      NodeAST_add_mem_operand(node, ea, NULL);
    {
      char imm[16];
      sprintf(imm, "%02x", *(uint8_t *)&text_mem[a + 2]);
      NodeAST_add_imm_operand(node, imm, *(uint8_t *)&text_mem[a + 2]);
    }
    il = 2 + *ds;
    break;

  case 0b11:
    print4b(text_mem, a, 2 + *ds + 1, line);
    int8_t imm8 = text_mem[a + 2 + *ds];
    sprintf(line + strlen(line), "%s %s, %s%x", instr,
            mod == 3 ? REG16[rm] : ea, imm8 < 0 ? "-" : "",
            imm8 < 0 ? -imm8 : imm8);
    NodeAST_set_opcode(node, instr);
    if (mod == 3)
      NodeAST_add_reg_operand(node, REG16[rm], rm);
    else
      NodeAST_add_mem_operand(node, ea, NULL);
    {
      char imm[16];
      sprintf(imm, "%s%x", imm8 < 0 ? "-" : "", imm8 < 0 ? -imm8 : imm8);
      NodeAST_add_imm_operand(node, imm, (uint16_t)imm8);
    }
    il = 2 + *ds + 1;
    break;
  }
  NodeAST_finalize(node, line, il);
  store_asm_line(a, line);
}

void printvw(size_t a, char *instr, unsigned char LSB4, size_t *ds,
             NodeAST *node) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  unsigned char v = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;

  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  if (!v) {
    if (!w) {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, 1", instr, REG8[rm]);
      else
        sprintf(line + strlen(line), "%s %s, 1", instr, ea);
    } else {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, 1", instr, REG16[rm]);
      else
        sprintf(line + strlen(line), "%s %s, 1", instr, ea);
    }
  } else {
    if (!w) {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, cl", instr, REG8[rm]);
      else
        sprintf(line + strlen(line), "%s byte %s, cl", instr, ea);
    } else {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, cl", instr, REG16[rm]);
      else
        sprintf(line + strlen(line), "%s %s, cl", instr, ea);
    }
  }
  NodeAST_set_opcode(node, instr);
  if (mod == 0b11)
    NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
  else
    NodeAST_add_mem_operand(node, ea, NULL);
  if (v)
    NodeAST_add_reg_operand(node, "cl", 10);
  else {
    char imm[8];
    sprintf(imm, "1");
    NodeAST_add_imm_operand(node, imm, 1);
  }
  NodeAST_finalize(node, line, 2 + *ds);
  store_asm_line(a, line);
  free(ea);
}

void printdw(size_t a, char *instr, unsigned char LSB4, size_t *ds,
             NodeAST *node) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  unsigned char d = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char reg = (text_mem[a + 1] >> 3) & 0b111;
  unsigned char rm = text_mem[a + 1] & 0b111;

  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  sprintf(line + strlen(line), "%s %s, %s", instr,
          d ? (w ? REG16[reg] : REG8[reg])
            : (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea),
          d ? (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea)
            : (w ? REG16[reg] : REG8[reg]));
  NodeAST_set_opcode(node, instr);
  if (d) {
    NodeAST_add_reg_operand(node, w ? REG16[reg] : REG8[reg],
                            w ? reg : reg + 8);
    if (mod == 3)
      NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
    else
      NodeAST_add_mem_operand(node, ea, NULL);
  } else {
    if (mod == 3)
      NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
    else
      NodeAST_add_mem_operand(node, ea, NULL);
    NodeAST_add_reg_operand(node, w ? REG16[reg] : REG8[reg],
                            w ? reg : reg + 8);
  }
  NodeAST_finalize(node, line, 2 + *ds);
  store_asm_line(a, line);
  free(ea);
}

void print_immtoacc(size_t a, char *instr, unsigned char w, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2 + w, line);

  if (w) {
    sprintf(line + strlen(line), "%s ax, %04x", instr,
            text_mem[a + 1] + (text_mem[a + 2] << 8));
  } else {
    sprintf(line + strlen(line), "%s al, %x", instr, text_mem[a + 1]);
  }
  NodeAST_set_opcode(node, instr);
  NodeAST_add_reg_operand(node, w ? "ax" : "al", w ? AX : AX + 8);
  {
    char imm[16];
    if (w)
      sprintf(imm, "%04x", text_mem[a + 1] + (text_mem[a + 2] << 8));
    else
      sprintf(imm, "%x", text_mem[a + 1]);
    NodeAST_add_imm_operand(node, imm,
                            w ? text_mem[a + 1] + (text_mem[a + 2] << 8)
                              : text_mem[a + 1]);
  }
  NodeAST_finalize(node, line, 2 + w);
  store_asm_line(a, line);
}

void print_immtorm(size_t a, char *instr, char *ea, unsigned char LSB4,
                   size_t *ds, NodeAST *node) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 3 + w + *ds, line);

  if (w)
    sprintf(line + strlen(line), "%s %s, %04x", instr,
            mod == 3 ? REG16[rm] : ea, *(uint16_t *)&text_mem[a + 2]);
  else
    sprintf(line + strlen(line), "%s %s, %02x", instr, mod == 3 ? REG8[rm] : ea,
            *(uint8_t *)&text_mem[a + 2]);
  NodeAST_set_opcode(node, instr);
  if (mod == 3)
    NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
  else
    NodeAST_add_mem_operand(node, ea, NULL);
  {
    char imm[16];
    if (w)
      sprintf(imm, "%04x", *(uint16_t *)&text_mem[a + 2]);
    else
      sprintf(imm, "%02x", *(uint8_t *)&text_mem[a + 2]);
    NodeAST_add_imm_operand(node, imm,
                            w ? *(uint16_t *)&text_mem[a + 2]
                              : *(uint8_t *)&text_mem[a + 2]);
  }
  NodeAST_finalize(node, line, 3 + w + *ds);
  store_asm_line(a, line);
}

void print_rmtoimm(size_t a, char *instr, char *ea, unsigned char LSB4,
                   size_t *ds, NodeAST *node) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 3 + w + *ds, line);

  if (w) {
    if (mod == 0b11) {
      sprintf(line + strlen(line), "%s %s, %04x", instr, REG16[rm],
              *(uint16_t *)&text_mem[a + 2]);
    } else {
      sprintf(line + strlen(line), "%s %s, %04x", instr, ea,
              *(uint16_t *)&text_mem[a + 2 + *ds]);
    }
  } else {
    if (mod == 0b11) {
      sprintf(line + strlen(line), "%s %s, %02x", instr, REG8[rm],
              *(uint8_t *)&text_mem[a + 2]);
    } else {
      sprintf(line + strlen(line), "%s %s, %02x", instr, ea,
              *(uint8_t *)&text_mem[a + 2]);
    }
  }
  NodeAST_set_opcode(node, instr);
  if (mod == 3)
    NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
  else
    NodeAST_add_mem_operand(node, ea, NULL);
  {
    char imm[16];
    if (w)
      sprintf(imm, "%04x", *(uint16_t *)&text_mem[a + 2]);
    else
      sprintf(imm, "%02x", *(uint8_t *)&text_mem[a + 2]);
    NodeAST_add_imm_operand(node, imm,
                            w ? *(uint16_t *)&text_mem[a + 2]
                              : *(uint8_t *)&text_mem[a + 2]);
  }
  NodeAST_finalize(node, line, 3 + w + *ds);
  store_asm_line(a, line);
}

void print_rm16(size_t a, char *instr, unsigned char rm, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);

  sprintf(line + strlen(line), "%s %s", instr, REG16[rm]);
  NodeAST_set_opcode(node, instr);
  NodeAST_add_reg_operand(node, REG16[rm], rm);
  NodeAST_finalize(node, line, 1);
  store_asm_line(a, line);
}

void print_condjmps(size_t a, char *instr, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + (signed char)text_mem[a + 1]);
  NodeAST_set_opcode(node, instr);
  {
    char imm[16];
    sprintf(imm, "%04x", IP + (signed char)text_mem[a + 1]);
    NodeAST_add_imm_operand(node, imm, IP + (signed char)text_mem[a + 1]);
  }
  NodeAST_finalize(node, line, 2);
  store_asm_line(a, line);
}

void print_repmanip(size_t a, char *instr, unsigned char w, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "rep %s%c", instr, w ? 'w' : 'b');
  NodeAST_set_opcode(node, "rep");
  NodeAST_add_imm_operand(node, w ? "1" : "0", w);
  NodeAST_finalize(node, line, 2);
  store_asm_line(a, line);
}

void print_single(size_t a, char *instr, size_t il, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, il, line);
  sprintf(line + strlen(line), "%s", instr);
  NodeAST_set_opcode(node, instr);
  NodeAST_finalize(node, line, il);
  store_asm_line(a, line);
}

void print_portfixed(size_t a, char *instr, unsigned char w, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %s, %x", instr, w ? "ax" : "al",
          text_mem[a + 1]);
  NodeAST_set_opcode(node, instr);
  NodeAST_add_reg_operand(node, w ? "ax" : "al", w ? AX : AX + 8);
  {
    char imm[8];
    sprintf(imm, "%x", text_mem[a + 1]);
    NodeAST_add_imm_operand(node, imm, text_mem[a + 1]);
  }
  NodeAST_finalize(node, line, 2);
  store_asm_line(a, line);
}

void print_portvar(size_t a, char *instr, unsigned char w, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);
  sprintf(line + strlen(line), "%s %s, dx", instr, w ? "ax" : "al");
  NodeAST_set_opcode(node, instr);
  NodeAST_add_reg_operand(node, w ? "ax" : "al", w ? AX : AX + 8);
  NodeAST_add_reg_operand(node, "dx", DX);
  NodeAST_finalize(node, line, 1);
  store_asm_line(a, line);
}

void print_loop(size_t a, char *instr, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + *(int8_t *)&text_mem[a + 1]);
  NodeAST_set_opcode(node, instr);
  {
    uint16_t target = IP + *(int8_t *)&text_mem[a + 1];
    char imm[16];
    sprintf(imm, "%04x", target);
    NodeAST_add_imm_operand(node, imm, target);
  }
  NodeAST_finalize(node, line, 2);
  store_asm_line(a, line);
}

void print_calljmp(size_t a, char *instr, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 3, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + *(int16_t *)&text_mem[a + 1]);
  NodeAST_set_opcode(node, instr);
  {
    uint16_t target = IP + *(int16_t *)&text_mem[a + 1];
    char imm[16];
    sprintf(imm, "%04x", target);
    NodeAST_add_imm_operand(node, imm, target);
  }
  NodeAST_finalize(node, line, 3);
  store_asm_line(a, line);
}

void print_stringmanip(size_t a, char *instr, unsigned char w, NodeAST *node) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);
  sprintf(line + strlen(line), "%s%c", instr, w ? 'w' : 'b');
  NodeAST_set_opcode(node, instr);
  NodeAST_finalize(node, line, 1);
  store_asm_line(a, line);
}

void print_fw(size_t a, char *instr, char *ea, unsigned char LSB4, size_t *ds,
              NodeAST *node) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2 + *ds, line);

  if (!w) {
    if (mod == 0b11)
      sprintf(line + strlen(line), "%s %s", instr, REG8[rm]);
    else
      sprintf(line + strlen(line), "%s %s", instr, ea);
  } else {
    if (mod == 0b11)
      sprintf(line + strlen(line), "%s %s", instr, REG16[rm]);
    else
      sprintf(line + strlen(line), "%s %s", instr, ea);
  }
  NodeAST_set_opcode(node, instr);
  if (mod == 0b11)
    NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm], w ? rm : rm + 8);
  else
    NodeAST_add_mem_operand(node, ea, NULL);
  NodeAST_finalize(node, line, 2 + *ds);
  store_asm_line(a, line);
}

void print_frm(size_t a, char *instr, char *ea, size_t *ds, NodeAST *node) {
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  sprintf(line + strlen(line), "%s %s", instr, mod == 3 ? REG16[rm] : ea);
  NodeAST_set_opcode(node, instr);
  if (mod == 3)
    NodeAST_add_reg_operand(node, REG16[rm], rm);
  else
    NodeAST_add_mem_operand(node, ea, NULL);
  NodeAST_finalize(node, line, 2 + *ds);
  store_asm_line(a, line);
}
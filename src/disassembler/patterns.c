#include "patterns.h"

// Global variables
unsigned char d, w, s, v, z;
unsigned char mod, reg, rm;
size_t ds = 0;

void f0x0(size_t a, unsigned char LSB4, NodeAST *node) {
  if (((LSB4 >> 3) & 0b1)) {
    if ((LSB4 >> 2) & 0b1) {
      // OR: imm -> acc
      print_immtoacc(a, "or", LSB4 & 0b1, node);
    } else {
      // OR: r/m & reg <->
      printdw(a, "or", LSB4, &ds, node);
    }
  } else {
    if ((LSB4 >> 2) & 0b1) {
      // ADD: imm -> acc
      print_immtoacc(a, "add", LSB4 & 0b1, node);
    } else {
      // ADD: r/m & reg <->
      printdw(a, "add", LSB4, &ds, node);
    }
  }
}

void f0x1(size_t a, unsigned char LSB4, NodeAST *node) {
  switch ((LSB4 >> 2) & 0b11) {
  case 0b00:
    // ADC: r/m & reg <->
    printdw(a, "adc", LSB4, &ds, node);
    break;

  case 0b01:
    if (!((LSB4 >> 1) & 0b1)) {
      // ADC: imm -> acc
      print_immtoacc(a, "adc", LSB4 & 0b1, node);
    }
    break;

  case 0b10:
    // SBB: r/m & reg <->
    printdw(a, "sbb", LSB4, &ds, node);
    break;

  case 0b11:
    // SBB: imm <- acc
    print_immtoacc(a, "sbb", LSB4 & 0b1, node);
    break;
  }
}

void f0x2(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  switch (LSB4 & 0b1111) {
  case 0xf:
    // DAS
    print_single(a, "das", 1, node);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // AND: r/m & reg <->
      printdw(a, "and", LSB4, &ds, node);
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // AND: imm -> acc
        print_immtoacc(a, "and", LSB4 & 0b1, node);
      }
      break;

    case 0b10:
      // SUB: r/m & reg <->
      printdw(a, "sub", LSB4, &ds, node);
      break;

    case 0b11:
      // SUB: imm. <- acc
      w = LSB4 & 0b1;
      print4b(text_mem, a, 2 + w, line);

      // Buffer to store the formatted string

      if (!w) {
        sprintf(line + strlen(line), "sub al, %02x", text_mem[a + 1]);
      } else {
        sprintf(line + strlen(line), "sub ax, %04x",
                *(uint16_t *)(text_mem + a + 1));
      }
      NodeAST_set_opcode(node, "sub");
      NodeAST_add_reg_operand(node, LSB4 & 0b1 ? "ax" : "al",
                              LSB4 & 0b1 ? AX : AX + 8);
      {
        char imm[16];
        if (!w)
          sprintf(imm, "%02x", text_mem[a + 1]);
        else
          sprintf(imm, "%04x", *(uint16_t *)(text_mem + a + 1));
        NodeAST_add_imm_operand(
            node, imm, !w ? text_mem[a + 1] : *(uint16_t *)(text_mem + a + 1));
      }
      NodeAST_finalize(node, line, 2 + w);
      store_asm_line(a, line);
      break;
    }

    break;
  }
}

void f0x3(size_t a, unsigned char LSB4, NodeAST *node) {
  switch (LSB4 & 0b1111) {
  case 0xf:
    // AAS
    print_single(a, "aas", 1, node);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // XOR: r/m & reg <->
      printdw(a, "xor", LSB4, &ds, node);
      break;

    case 0b01:
      // XOR: imm -> acc
      print_immtoacc(a, "xor", LSB4 & 0b1, node);
      break;

    case 0b10:
      // CMP: r/m & reg
      printdw(a, "cmp", LSB4, &ds, node);
      break;

    case 0b11:
      // CMP: imm. + acc
      print_immtoacc(a, "cmp", LSB4 & 0b1, node);
      break;
    }
    break;
  }
}

void f0x4(size_t a, unsigned char LSB4, NodeAST *node) {
  if ((LSB4 >> 3) & 0b1) {
    // DEC: r/m
    print_rm16(a, "dec", LSB4 & 0b111, node);
  } else {
    // INC: r/m
    print_rm16(a, "inc", LSB4 & 0b111, node);
  }
}

void f0x5(size_t a, unsigned char LSB4, NodeAST *node) {
  if ((LSB4 >> 3) & 0b1) {
    // POP: reg
    print_rm16(a, "pop", LSB4 & 0b111, node);
  } else {
    // PUSH: reg
    print_rm16(a, "push", LSB4 & 0b111, node);
  }
}

void f0x6(size_t a, unsigned char LSB4, NodeAST *node) {
  LSB4 &= 0b111; // to remove unused variable warning
  print_single(a, "(undefined)", 1, node);
}

void f0x7(size_t a, unsigned char LSB4, NodeAST *node) {
  switch (LSB4 & 0b1111) {
  case 0x0:
    // JO
    print_condjmps(a, "jo", node);
    break;

  case 0x1:
    // JNO
    print_condjmps(a, "jno", node);
    break;

  case 0x2:
    // JB/JNEA
    print_condjmps(a, "jb", node);
    break;

  case 0x3:
    // JNB/JAE
    print_condjmps(a, "jnb", node);
    break;

  case 0x4:
    // JE/JZ
    print_condjmps(a, "je", node);
    break;

  case 0x5:
    // JNE/JNZ
    print_condjmps(a, "jne", node);
    break;

  case 0x6:
    // JBE/JNA
    print_condjmps(a, "jbe", node);
    break;

  case 0x7:
    // JNBE/JA
    print_condjmps(a, "jnbe", node);
    break;

  case 0x8:
    // JS
    print_condjmps(a, "js", node);
    break;

  case 0x9:
    // JNS
    // print_condjmps(a, "jns");
    print_single(a, "(undefined)", 1, node);

    break;

  case 0xa:
    // JP/JPE
    // print_condjmps(a, "jp");
    print_single(a, "(undefined)", 1, node);
    break;

  case 0xb:
    // JNP/JPO
    print_condjmps(a, "jnp", node);
    break;

  case 0xc:
    // JL/JNGE
    print_condjmps(a, "jl", node);
    break;

  case 0xd:
    // JNL/JGE
    print_condjmps(a, "jnl", node);
    break;

  case 0xe:
    // JLE/JNG
    print_condjmps(a, "jle", node);
    break;

  case 0xf:
    // JNLE/JG
    print_condjmps(a, "jnle", node);
    break;
  }
}

void f0x8(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0xd:
    // LEA: reg <- EA
    mod = (text_mem[a + 1] >> 6) & 0b11;
    reg = (text_mem[a + 1] >> 3) & 0b111;
    rm = text_mem[a + 1] & 0b111;
    get_adm(a + 2, mod, rm, ea, &ds);
    print4b(text_mem, a, 2 + ds, line);
    sprintf(line + strlen(line), "lea %s, %s", REG16[reg], ea);
    NodeAST_set_opcode(node, "lea");
    NodeAST_add_reg_operand(node, REG16[reg], reg);
    NodeAST_add_mem_operand(node, ea, NULL);
    NodeAST_finalize(node, line, 2 + ds);
    store_asm_line(a, line);
    break;

  case 0xf:
    // POP: r/m
    print4b(text_mem, a, 2, line);
    IP += 2;
    sprintf(line + strlen(line), "0x8f is not implemented");
    NodeAST_set_opcode(node, "(undefined)");
    NodeAST_finalize(node, line, 2);
    store_asm_line(a, line);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        printsw(a, "add", ea, LSB4, &ds, node);
        break;
      case 0b001:
        print_rmtoimm(a, "or", ea, LSB4, &ds, node);
        break;
      case 0b010:
        printsw(a, "adc", ea, LSB4, &ds, node);
        break;
      case 0b011:
        printsw(a, "sbb", ea, LSB4, &ds, node);
        break;
      case 0b100:
        print_rmtoimm(a, "and", ea, LSB4, &ds, node);
        break;
      case 0b101:
        printsw(a, "sub", ea, LSB4, &ds, node);
        break;
      case 0b110:
        print_immtorm(a, "xor", ea, LSB4, &ds, node);
        break;
      case 0b111:
        printsw(a, "cmp", ea, LSB4, &ds, node);
        break;
      }
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        printdw(a, "test", LSB4, &ds, node);
      } else {
        w = LSB4 & 0b1;
        mod = (text_mem[a + 1] >> 6) & 0b11;
        reg = (text_mem[a + 1] >> 3) & 0b111;
        rm = text_mem[a + 1] & 0b111;
        get_adm(a + 2, mod, rm, ea, &ds);
        print4b(text_mem, a, 2 + ds, line);

        if (!w) {
          if (mod == 0b11)
            sprintf(line + strlen(line), "xchg %s, %s", REG8[rm], REG8[reg]);
          else
            sprintf(line + strlen(line), "xchg %s, %s", ea, REG8[reg]);
        } else {
          if (mod == 0b11)
            sprintf(line + strlen(line), "xchg %s, %s", REG16[rm], REG16[reg]);
          else
            sprintf(line + strlen(line), "xchg %s, %s", ea, REG16[reg]);
        }
        NodeAST_set_opcode(node, "xchg");
        if (mod == 0b11) {
          NodeAST_add_reg_operand(node, w ? REG16[rm] : REG8[rm],
                                  w ? rm : rm + 8);
        } else {
          NodeAST_add_mem_operand(node, ea, NULL);
        }
        NodeAST_add_reg_operand(node, w ? REG16[reg] : REG8[reg],
                                w ? reg : reg + 8);
        NodeAST_finalize(node, line, 2 + ds);
        store_asm_line(a, line);
      }
      break;

    case 0b10:
      printdw(a, "mov", LSB4, &ds, node);
      break;

    case 0b11:
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b11;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);
      print4b(text_mem, a, 2 + ds, line);

      switch (LSB4 & 0b11) {
      case 0b00:
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", REG16[rm], SEGREG[reg]);
        else
          sprintf(line + strlen(line), "mov %s, %s", ea, SEGREG[reg]);
        NodeAST_set_opcode(node, "mov");
        if (mod == 0b11)
          NodeAST_add_reg_operand(node, REG16[rm], rm);
        else
          NodeAST_add_mem_operand(node, ea, NULL);
        NodeAST_add_reg_operand(node, SEGREG[reg], reg);
        NodeAST_finalize(node, line, 2 + ds);
        store_asm_line(a, line);
        break;

      case 0b10:
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], REG16[rm]);
        else
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], ea);
        NodeAST_set_opcode(node, "mov");
        NodeAST_add_reg_operand(node, SEGREG[reg], reg);
        if (mod == 0b11)
          NodeAST_add_reg_operand(node, REG16[rm], rm);
        else
          NodeAST_add_mem_operand(node, ea, NULL);
        NodeAST_finalize(node, line, 2 + ds);
        store_asm_line(a, line);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}

void f0x9(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    switch (LSB4 & 0b1111) {
    case 0x8:
      // CBW
      print_single(a, "cbw", 1, node);
      break;

    case 0x9:
      // CWD
      print_single(a, "cwd", 1, node);
      break;

    case 0xb:
      // WAIT
      // print_single(a, "wait", 1);
      print_single(a, "(undefined)", 1, node);
      break;

    case 0xc:
      // PUSHF
      print_single(a, "pushf", 1, node);
      break;

    case 0xd:
      // POPF
      print_single(a, "popf", 1, node);
      break;

    case 0xe:
      // SAHF
      print_single(a, "sahf", 1, node);
      break;

    case 0xf:
      // LAHF
      print_single(a, "lahf", 1, node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
  } else {
    // XCHG: reg + acc.
    print4b(text_mem, a, 1, line);
    sprintf(line + strlen(line), "xchg %s, ax", REG16[LSB4 & 0b111]);
    NodeAST_set_opcode(node, "xchg");
    NodeAST_add_reg_operand(node, REG16[LSB4 & 0b111], LSB4 & 0b111);
    NodeAST_add_reg_operand(node, "ax", AX);
    NodeAST_finalize(node, line, 1);
    store_asm_line(a, line);
  }

  free(ea);
}

void f0xa(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  w = LSB4 & 0b1;
  switch ((LSB4 >> 1) & 0b111) {
  case 0b000:
    // MOV: mem -> acc
    print4b(text_mem, a, 3, line);

    sprintf(line + strlen(line), "mov %s, [%04x]", w ? "ax" : "al",
            *(uint16_t *)&text_mem[a + 1]);
    NodeAST_set_opcode(node, "mov");
    NodeAST_add_reg_operand(node, w ? "ax" : "al", w ? AX : AX + 8);
    {
      char mem[16];
      sprintf(mem, "[%04x]", *(uint16_t *)&text_mem[a + 1]);
      NodeAST_add_mem_operand(node, mem, NULL);
    }
    NodeAST_finalize(node, line, 3);
    store_asm_line(a, line);

    break;

  case 0b001:
    // MOV: acc -> mem
    print4b(text_mem, a, 3, line);
    sprintf(line + strlen(line), "mov [%04x], %s",
            *(uint16_t *)&text_mem[a + 1], w ? "ax" : "al");
    NodeAST_set_opcode(node, "mov");
    {
      char mem[16];
      sprintf(mem, "[%04x]", *(uint16_t *)&text_mem[a + 1]);
      NodeAST_add_mem_operand(node, mem, NULL);
    }
    NodeAST_add_reg_operand(node, w ? "ax" : "al", w ? AX : AX + 8);
    NodeAST_finalize(node, line, 3);
    store_asm_line(a, line);

    break;

  case 0b010:
    // MOVS
    print_stringmanip(a, "movs", LSB4 & 0b1, node);
    break;

  case 0b011:
    // CMPS
    print_stringmanip(a, "cmps", LSB4 & 0b1, node);
    break;

  case 0b100:
    // TEST: imm & acc
    print_immtoacc(a, "test", LSB4 & 0b1, node);
    break;

  case 0b101:
    // STOS
    print_stringmanip(a, "stos", LSB4 & 0b1, node);
    break;

  case 0b110:
    // LODS
    print_stringmanip(a, "lods", LSB4 & 0b1, node);
    break;

  case 0b111:
    // SCAS
    print_stringmanip(a, "scas", LSB4 & 0b1, node);
    break;
  }

  free(ea);
}

void f0xb(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // MOV: imm -> register
  w = (LSB4 >> 3) & 0b1;
  reg = LSB4 & 0b111;

  print4b(text_mem, a, 2 + w, line);

  if (!w) {
    sprintf(line + strlen(line), "mov %s, %x", REG8[reg], text_mem[a + 1]);
  } else {
    sprintf(line + strlen(line), "mov %s, %04x", REG16[reg],
            *(uint16_t *)&text_mem[a + 1]);
  }
  NodeAST_set_opcode(node, "mov");
  NodeAST_add_reg_operand(node, w ? REG16[reg] : REG8[reg], w ? reg : reg + 8);
  {
    char imm[16];
    if (!w)
      sprintf(imm, "%x", text_mem[a + 1]);
    else
      sprintf(imm, "%04x", *(uint16_t *)&text_mem[a + 1]);
    NodeAST_add_imm_operand(
        node, imm, !w ? text_mem[a + 1] : *(uint16_t *)&text_mem[a + 1]);
  }
  NodeAST_finalize(node, line, 2 + w);
  store_asm_line(a, line);
  free(ea);
}

void f0xc(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b011:
    // MOV: imm -> r/m
    w = LSB4 & 0b1;
    mod = (text_mem[a + 1] >> 6) & 0b11;
    reg = (text_mem[a + 1] >> 3) & 0b111;
    rm = text_mem[a + 1] & 0b111;
    get_adm(a + 2, mod, rm, ea, &ds);

    if (w) {
      print4b(text_mem, a, 4 + ds, line);
      if (mod == 0b11) {
        sprintf(line + strlen(line), "mov %s, %04x", REG16[rm],
                *(uint16_t *)&text_mem[a + 3]);
      } else {
        sprintf(line + strlen(line), "mov %s, %04x", ea,
                *(uint16_t *)&text_mem[a + 2 + ds]);
      }
    } else {
      print4b(text_mem, a, 3 + ds, line);
      if (mod == 0b11) {
        sprintf(line + strlen(line), "mov byte %s, %x", REG8[rm],
                text_mem[a + 2 + ds]);
      } else {
        sprintf(line + strlen(line), "mov byte %s, %x", ea,
                text_mem[a + 2 + ds]);
      }
    }
    NodeAST_set_opcode(node, "mov");
    if (w) {
      if (mod == 0b11)
        NodeAST_add_reg_operand(node, REG16[rm], rm);
      else
        NodeAST_add_mem_operand(node, ea, NULL);
      {
        uint16_t value = *(uint16_t *)&text_mem[a + (mod == 0b11 ? 3 : 2 + ds)];
        char imm[16];
        sprintf(imm, "%04x", value);
        NodeAST_add_imm_operand(node, imm, value);
      }
    } else {
      if (mod == 0b11)
        NodeAST_add_reg_operand(node, REG8[rm], rm + 8);
      else
        NodeAST_add_mem_operand(node, ea, "byte");
      {
        uint8_t value = text_mem[a + 2 + ds];
        char imm[16];
        sprintf(imm, "%x", value);
        NodeAST_add_imm_operand(node, imm, value);
      }
    }
    NodeAST_finalize(node, line, w ? 4 + ds : 3 + ds);
    store_asm_line(a, line);

    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x2:
      // RET: winseg + immed -> SP
      print4b(text_mem, a, 3, line);
      sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a + 1]);
      NodeAST_set_opcode(node, "ret");
      {
        uint16_t value = *(uint16_t *)&text_mem[a + 1];
        char imm[16];
        sprintf(imm, "%04x", value);
        NodeAST_add_imm_operand(node, imm, value);
      }
      NodeAST_finalize(node, line, 3);
      store_asm_line(a, line);

      break;

    case 0x3:
      // RET: winseg
      print_single(a, "ret", 1, node);
      break;

    case 0x4:
      // LDS: ptr -> DS
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, line);
      sprintf(line + strlen(line), "lds %s, %s", REG16[reg], ea);
      NodeAST_set_opcode(node, "lds");
      NodeAST_add_reg_operand(node, REG16[reg], reg);
      NodeAST_add_mem_operand(node, ea, NULL);
      NodeAST_finalize(node, line, 2 + ds);
      store_asm_line(a, line);

      break;

    case 0x5:
      // LES: ptr -> ES
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, line);
      sprintf(line + strlen(line), "les %s, %s", REG16[reg], ea);
      NodeAST_set_opcode(node, "les");
      NodeAST_add_reg_operand(node, REG16[reg], reg);
      NodeAST_add_mem_operand(node, ea, NULL);
      NodeAST_finalize(node, line, 2 + ds);
      store_asm_line(a, line);

      break;

    case 0xa:
      // RET: intersegment + immed -> SP
      // print4b(text_mem,a, 3);
      // sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a +
      // 1]);
      print_single(a, "(undefined)", 1, node);

      break;

    case 0xb:
      // RET: intersegement
      print_single(a, "ret (intersegment)", 1, node);
      break;

    case 0xd:
      if (LSB4 & 0b1) {
        // INT: spec
        print4b(text_mem, a, 2, line);
        sprintf(line + strlen(line), "int %x", text_mem[a + 1]);
        NodeAST_set_opcode(node, "int");
        {
          char imm[16];
          sprintf(imm, "%x", text_mem[a + 1]);
          NodeAST_add_imm_operand(node, imm, text_mem[a + 1]);
        }
        NodeAST_finalize(node, line, 2);
        store_asm_line(a, line);

      } else {
        // INT: unspec, type = 3
        print_single(a, "int (3)", 1, node);
      }
      break;

    case 0xe:
      // INTO
      print_single(a, "into", 1, node);
      break;

    case 0xf:
      // IRET
      print_single(a, "iret", 1, node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
    break;
  }

  free(ea);
}

void f0xd(size_t a, unsigned char LSB4, NodeAST *node) {
  switch (LSB4 & 0b1111) {
  case 0x4:
    // AAM
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aam", 2, node);
    break;

  case 0x5:
    // AAD
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aad", 2, node);
    break;

  case 0x7:
    // XLAT
    print_single(a, "xlat", 1, node);
    break;

  default:
    switch ((text_mem[a + 1] >> 3) & 0b111) {
    case 0b000:
      // ROL
      // printvw(a, "rol", LSB4);
      print_single(a, "(undefined)", 1, node);

      break;

    case 0b001:
      // ROR
      printvw(a, "ror", LSB4, &ds, node);
      break;

    case 0b010:
      // RCL
      printvw(a, "rcl", LSB4, &ds, node);
      break;

    case 0b011:
      // RCR
      printvw(a, "rcr", LSB4, &ds, node);
      break;

    case 0b100:
      // SHL/SAL
      printvw(a, "shl", LSB4, &ds, node);
      break;

    case 0b101:
      // SHR
      printvw(a, "shr", LSB4, &ds, node);
      break;

    case 0b111:
      // SAR
      printvw(a, "sar", LSB4, &ds, node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
    break;
  }
}

void f0xe(size_t a, unsigned char LSB4, NodeAST *node) {
  switch ((LSB4 >> 1) & 0b111) {
  case 0b010:
    // IN: fixed port
    print_portfixed(a, "in", LSB4 & 0b1, node);
    break;

  case 0b011:
    // OUT: fixed port
    print_portfixed(a, "out", LSB4 & 0b1, node);
    break;

  case 0b110:
    // IN: variable port
    print_portvar(a, "in", LSB4 & 0b1, node);
    break;

  case 0b111:
    // OUT: variable port
    print_portvar(a, "out", LSB4 & 0b1, node);
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOOPNZ/LOOPNE
      print_loop(a, "loopnz", node);
      break;

    case 0x1:
      // LOOPZ/LOOPE
      print_loop(a, "loopz", node);
      break;

    case 0x2:
      // LOOP
      print_loop(a, "loop", node);
      break;

    case 0x3:
      // JCXZ
      print_loop(a, "jcxz", node);
      break;

    case 0x8:
      // CALL: dirseg
      print_calljmp(a, "call", node);
      break;

    case 0x9:
      // JMP: dirseg
      print_calljmp(a, "jmp", node);
      break;

    case 0xb:
      // JMP SHORT: dirseg short
      print_loop(a, "jmp short", node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
    break;
  }
}

void f0xf(size_t a, unsigned char LSB4, NodeAST *node) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b001:
    // REP (z flag)
    z = LSB4 & 0b1;
    unsigned char p4 = text_mem[a + 1] & 0b1111;
    switch (p4 >> 1) {
    case 0b010:
      // REP + MOVS
      print_repmanip(a, "movs", p4 & 0b1, node);
      break;

    case 0b011:
      // REP + CMPS
      print_repmanip(a, "cmps", p4 & 0b1, node);
      break;

    case 0b101:
      // REP + STOS
      print_repmanip(a, "stos", p4 & 0b1, node);
      break;

    case 0b110:
      // REP + LODS
      print_repmanip(a, "lods", p4 & 0b1, node);
      break;

    case 0b111:
      // REP + SCAS
      print_repmanip(a, "scas", p4 & 0b1, node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
    break;

  case 0b011:
    w = LSB4 & 0b1;
    mod = (text_mem[a + 1] >> 6) & 0b11;
    rm = text_mem[a + 1] & 0b111;
    get_adm(a + 2, mod, rm, ea, &ds);

    switch ((text_mem[a + 1] >> 3) & 0b111) {
    case 0b000:
      // TEST: imm -> r/m
      print4b(text_mem, a, 3 + w + ds, line);

      if (!w) {
        if (mod == 0b11)
          sprintf(line + strlen(line), "test %s, %x", REG8[rm],
                  text_mem[a + 2]);
        else
          sprintf(line + strlen(line), "test byte %s, %x", ea,
                  text_mem[a + 2 + ds]); // to recheck byte
      } else {
        if (mod == 0b11)
          sprintf(line + strlen(line), "test %s, %04x", REG16[rm],
                  *(uint16_t *)&text_mem[a + 2]);
        else
          sprintf(line + strlen(line), "test %s, %04x", ea,
                  *(uint16_t *)&text_mem[a + 2 + ds]);
      }
      NodeAST_set_opcode(node, "test");
      if (!w) {
        if (mod == 0b11)
          NodeAST_add_reg_operand(node, REG8[rm], rm + 8);
        else
          NodeAST_add_mem_operand(node, ea, "byte");
        {
          uint8_t value = mod == 0b11 ? text_mem[a + 2] : text_mem[a + 2 + ds];
          char imm[16];
          sprintf(imm, "%x", value);
          NodeAST_add_imm_operand(node, imm, value);
        }
      } else {
        if (mod == 0b11)
          NodeAST_add_reg_operand(node, REG16[rm], rm);
        else
          NodeAST_add_mem_operand(node, ea, NULL);
        {
          uint16_t value = mod == 0b11 ? *(uint16_t *)&text_mem[a + 2]
                                       : *(uint16_t *)&text_mem[a + 2 + ds];
          char imm[16];
          sprintf(imm, "%04x", value);
          NodeAST_add_imm_operand(node, imm, value);
        }
      }
      NodeAST_finalize(node, line, 3 + w + ds);
      store_asm_line(a, line);

      break;

    case 0b010:
      // NOT
      print_fw(a, "not", ea, LSB4, &ds, node);
      break;

    case 0b011:
      // NEG
      // sprintf(line + strlen(line), "mod=%x, rm=%x  ", mod, rm);
      print_fw(a, "neg", ea, LSB4, &ds, node);
      break;

    case 0b100:
      // MUL
      print_fw(a, "mul", ea, LSB4, &ds, node);
      break;

    case 0b101:
      // IMUL
      print_fw(a, "imul", ea, LSB4, &ds, node);
      break;

    case 0b110:
      // DIV
      print_fw(a, "div", ea, LSB4, &ds, node);
      break;

    case 0b111:
      // IDIV
      print_fw(a, "idiv", ea, LSB4, &ds, node);
      break;

    default:
      print_single(a, "(undefined)", 1, node);
      break;
    }
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOCK
      // print_single(a + 1, "lock", 1);
      print_single(a, "(undefined)", 1, node);
      break;

    case 0x4:
      // HLT
      print_single(a, "hlt", 1, node);
      break;

    case 0x5:
      // CMC
      print_single(a, "cmc", 1, node);

      break;

    case 0x8:
      // CLC
      // print_single(a, "clc", 1);
      print_single(a, "(undefined)", 1, node);
      break;

    case 0x9:
      // STC
      // print_single(a, "stc", 1);
      print_single(a, "(undefined)", 1, node);
      break;

    case 0xa:
      // CLI
      print_single(a, "cli", 1, node);
      break;

    case 0xb:
      // STI
      print_single(a, "sti", 1, node);
      break;

    case 0xc:
      // CLD
      print_single(a, "cld", 1, node);
      break;

    case 0xd:
      // STD
      print_single(a, "std", 1, node);
      break;

    case 0xf:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        // INC: r/m
        print_frm(a, "inc", ea, &ds, node);
        break;

      case 0b001:
        // DEC: r/m
        print_frm(a, "dec", ea, &ds, node);
        break;

      case 0b010:
        // CALL: indw|seg
        print_frm(a, "call", ea, &ds, node);
        break;

      case 0b011:
        // CALL: indirect intersegment
        print_frm(a, "call (ind inter)", ea, &ds, node);
        break;

      case 0b100:
        // JMP: indw|seg
        print_frm(a, "jmp", ea, &ds, node);
        break;

      case 0b101:
        // JMP: indirect intersegment
        print_frm(a, "jmp (ind inter)", ea, &ds, node);
        break;

      case 0b110:
        // PUSH: r/m
        print_frm(a, "push", ea, &ds, node);
        break;

      default:
        print_single(a, "(undefined)", 1, node);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}
#include "patterns.h"

// Global variables
unsigned char d, w, s, v, z;
unsigned char mod, reg, rm;
size_t ds = 0;

void f0x0(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  if (((LSB4 >> 3) & 0b1)) {
    if ((LSB4 >> 2) & 0b1) {
      // OR: imm -> acc
      print_immtoacc(a, "or", LSB4 & 0b1, ip);
    } else {
      // OR: r/m & reg <->
      printdw(a, "or", ip, LSB4, &ds);
    }
  } else {
    if ((LSB4 >> 2) & 0b1) {
      // ADD: imm -> acc
      print_immtoacc(a, "add", LSB4 & 0b1, ip);
    } else {
      // ADD: r/m & reg <->
      printdw(a, "add", ip, LSB4, &ds);
    }
  }
}

void f0x1(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  switch ((LSB4 >> 2) & 0b11) {
  case 0b00:
    // ADC: r/m & reg <->
    printdw(a, "adc", ip, LSB4, &ds);
    break;

  case 0b01:
    if (!((LSB4 >> 1) & 0b1)) {
      // ADC: imm -> acc
      print_immtoacc(a, "adc", LSB4 & 0b1, ip);
    }
    break;

  case 0b10:
    // SBB: r/m & reg <->
    printdw(a, "sbb", ip, LSB4, &ds);
    break;

  case 0b11:
    // SBB: imm <- acc
    print_immtoacc(a, "sbb", LSB4 & 0b1, ip);
    break;
  }
}

void f0x2(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  switch (LSB4 & 0b1111) {
  case 0xf:
    // DAS
    print_single(a, "das", ip, 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // AND: r/m & reg <->
      printdw(a, "and", ip, LSB4, &ds);
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // AND: imm -> acc
        print_immtoacc(a, "and", LSB4 & 0b1, ip);
      }
      break;

    case 0b10:
      // SUB: r/m & reg <->
      printdw(a, "sub", ip, LSB4, &ds);
      break;

    case 0b11:
      // SUB: imm. <- acc
      w = LSB4 & 0b1;
      print4b(text_mem, a, 2 + w, ip, line);

      // Buffer to store the formatted string

      if (!w) {
        sprintf(line + strlen(line), "sub al, %02x", text_mem[a + 1]);
      } else {
        sprintf(line + strlen(line), "sub ax, %04x",
                *(uint16_t *)(text_mem + a + 1));
      }
      strncpy(ASM[a], line, sizeof(ASM[a]));

      break;
    }

    break;
  }
}

void f0x3(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  switch (LSB4 & 0b1111) {
  case 0xf:
    // AAS
    print_single(a, "aas", ip, 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // XOR: r/m & reg <->
      printdw(a, "xor", ip, LSB4, &ds);
      break;

    case 0b01:
      // XOR: imm -> acc
      print_immtoacc(a, "xor", LSB4 & 0b1, ip);
      break;

    case 0b10:
      // CMP: r/m & reg
      printdw(a, "cmp", ip, LSB4, &ds);
      break;

    case 0b11:
      // CMP: imm. + acc
      print_immtoacc(a, "cmp", LSB4 & 0b1, ip);
      break;
    }
    break;
  }
}

void f0x4(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // DEC: r/m
    print_rm16(a, "dec", LSB4 & 0b111, ip);
  } else {
    // INC: r/m
    print_rm16(a, "inc", LSB4 & 0b111, ip);
  }

  free(ea);
}

void f0x5(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // POP: reg
    print_rm16(a, "pop", LSB4 & 0b111, ip);
  } else {
    // PUSH: reg
    print_rm16(a, "push", LSB4 & 0b111, ip);
  }

  free(ea);
}

void f0x6(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  LSB4 &= 0b111; // to remove unused variable warning
  print_single(a, "(undefined)", ip, 1);
  free(ea);
}

void f0x7(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0x0:
    // JO
    print_condjmps(a, "jo", ip);
    break;

  case 0x1:
    // JNO
    print_condjmps(a, "jno", ip);
    break;

  case 0x2:
    // JB/JNEA
    print_condjmps(a, "jb", ip);
    break;

  case 0x3:
    // JNB/JAE
    print_condjmps(a, "jnb", ip);
    break;

  case 0x4:
    // JE/JZ
    print_condjmps(a, "je", ip);
    break;

  case 0x5:
    // JNE/JNZ
    print_condjmps(a, "jne", ip);
    break;

  case 0x6:
    // JBE/JNA
    print_condjmps(a, "jbe", ip);
    break;

  case 0x7:
    // JNBE/JA
    print_condjmps(a, "jnbe", ip);
    break;

  case 0x8:
    // JS
    print_condjmps(a, "js", ip);
    break;

  case 0x9:
    // JNS
    // print_condjmps(a, "jns", ip);
    print_single(a, "(undefined)", ip, 1);

    break;

  case 0xa:
    // JP/JPE
    // print_condjmps(a, "jp", ip);
    print_single(a, "(undefined)", ip, 1);
    break;

  case 0xb:
    // JNP/JPO
    print_condjmps(a, "jnp", ip);
    break;

  case 0xc:
    // JL/JNGE
    print_condjmps(a, "jl", ip);
    break;

  case 0xd:
    // JNL/JGE
    print_condjmps(a, "jnl", ip);
    break;

  case 0xe:
    // JLE/JNG
    print_condjmps(a, "jle", ip);
    break;

  case 0xf:
    // JNLE/JG
    print_condjmps(a, "jnle", ip);
    break;
  }

  free(ea);
}

void f0x8(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0xd:
    // LEA: reg <- EA
    mod = (text_mem[a + 1] >> 6) & 0b11;
    reg = (text_mem[a + 1] >> 3) & 0b111;
    rm = text_mem[a + 1] & 0b111;
    get_adm(a + 2, mod, rm, ea, &ds);
    print4b(text_mem, a, 2 + ds, ip, line);
    sprintf(line + strlen(line), "lea %s, %s", REG16[reg], ea);
    strncpy(ASM[a], line, sizeof(ASM[a]));

    break;

  case 0xf:
    // POP: r/m
    *ip += 2;
    sprintf(line + strlen(line),
            "0x8f is not implemented"); // should be POP r/m
    strncpy(ASM[a], line, sizeof(ASM[a]));

    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        // ADD: imm -> r/m
        printsw(a, "add", ip, ea, LSB4, &ds);
        break;

      case 0b001:
        // OR: imm <- r/m
        print_rmtoimm(a, "or", ip, ea, LSB4, &ds);
        break;

      case 0b010:
        // ADC: imm -> r/m
        printsw(a, "adc", ip, ea, LSB4, &ds);
        break;

      case 0b011:
        // SBB: imm <- r/m
        printsw(a, "sbb", ip, ea, LSB4, &ds);
        break;

      case 0b100: // NO SW
        // AND: imm <- r/m
        print_rmtoimm(a, "and", ip, ea, LSB4, &ds);
        break;

      case 0b101:
        // SUB: imm <- r/m
        printsw(a, "sub", ip, ea, LSB4, &ds);
        break;

      case 0b110: // SW not needed
        // XOR: imm -> r/m
        print_immtorm(a, "xor", ip, ea, LSB4, &ds);
        break;

      case 0b111:
        // CMP: imm + r/m
        printsw(a, "cmp", ip, ea, LSB4, &ds);
        break;
      }
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // TEST: r/m & reg <->
        printdw(a, "test", ip, LSB4, &ds);
      } else {
        // XCHG: r/m + reg
        w = LSB4 & 0b1;
        mod = (text_mem[a + 1] >> 6) & 0b11;
        reg = (text_mem[a + 1] >> 3) & 0b111;
        rm = text_mem[a + 1] & 0b111;
        get_adm(a + 2, mod, rm, ea, &ds);
        print4b(text_mem, a, 2 + ds, ip, line);

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
        strncpy(ASM[a], line, sizeof(ASM[a]));
      }
      break;

    case 0b10:
      // MOV: r/m <-> reg
      printdw(a, "mov", ip, LSB4, &ds);
      break;

    case 0b11:
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b11;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);
      print4b(text_mem, a, 2 + ds, ip, line);

      switch (LSB4 & 0b11) {
      case 0b00:
        // MOV: sreg -> r/m
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", REG16[rm], SEGREG[reg]);
        else
          sprintf(line + strlen(line), "mov %s, %s", ea, SEGREG[reg]);
        strncpy(ASM[a], line, sizeof(ASM[a]));
        break;

      case 0b10:
        // MOV: r/m -> sreg
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], REG16[rm]);
        else
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], ea);
        strncpy(ASM[a], line, sizeof(ASM[a]));
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}

void f0x9(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    switch (LSB4 & 0b1111) {
    case 0x8:
      // CBW
      print_single(a, "cbw", ip, 1);
      break;

    case 0x9:
      // CWD
      print_single(a, "cwd", ip, 1);
      break;

    case 0xb:
      // WAIT
      // print_single(a, "wait", ip, 1);
      print_single(a, "(undefined)", ip, 1);
      break;

    case 0xc:
      // PUSHF
      print_single(a, "pushf", ip, 1);
      break;

    case 0xd:
      // POPF
      print_single(a, "popf", ip, 1);
      break;

    case 0xe:
      // SAHF
      print_single(a, "sahf", ip, 1);
      break;

    case 0xf:
      // LAHF
      print_single(a, "lahf", ip, 1);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
      break;
    }
  } else {
    // XCHG: reg + acc.
    print4b(text_mem, a, 1, ip, line);
    sprintf(line + strlen(line), "xchg %s, ax", REG16[LSB4 & 0b111]);
    strncpy(ASM[a], line, sizeof(ASM[a]));
  }

  free(ea);
}

void f0xa(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  w = LSB4 & 0b1;
  switch ((LSB4 >> 1) & 0b111) {
  case 0b000:
    // MOV: mem -> acc
    print4b(text_mem, a, 3, ip, line);

    sprintf(line + strlen(line), "mov %s, [%04x]", w ? "ax" : "al",
            *(uint16_t *)&text_mem[a + 1]);
    strncpy(ASM[a], line, sizeof(ASM[a]));

    break;

  case 0b001:
    // MOV: acc -> mem
    print4b(text_mem, a, 3, ip, line);
    sprintf(line + strlen(line), "mov [%04x], %s",
            *(uint16_t *)&text_mem[a + 1], w ? "ax" : "al");
    strncpy(ASM[a], line, sizeof(ASM[a]));

    break;

  case 0b010:
    // MOVS
    print_stringmanip(a, "movs", LSB4 & 0b1, ip);
    break;

  case 0b011:
    // CMPS
    print_stringmanip(a, "cmps", LSB4 & 0b1, ip);
    break;

  case 0b100:
    // TEST: imm & acc
    print_immtoacc(a, "test", LSB4 & 0b1, ip);
    break;

  case 0b101:
    // STOS
    print_stringmanip(a, "stos", LSB4 & 0b1, ip);
    break;

  case 0b110:
    // LODS
    print_stringmanip(a, "lods", LSB4 & 0b1, ip);
    break;

  case 0b111:
    // SCAS
    print_stringmanip(a, "scas", LSB4 & 0b1, ip);
    break;
  }

  free(ea);
}

void f0xb(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  // MOV: imm -> register
  w = (LSB4 >> 3) & 0b1;
  reg = LSB4 & 0b111;

  print4b(text_mem, a, 2 + w, ip, line);

  if (!w) {
    sprintf(line + strlen(line), "mov %s, %x", REG8[reg], text_mem[a + 1]);
  } else {
    sprintf(line + strlen(line), "mov %s, %04x", REG16[reg],
            *(uint16_t *)&text_mem[a + 1]);
  }

  strncpy(ASM[a], line, sizeof(ASM[a]));
  free(ea);
}

void f0xc(size_t a, unsigned char LSB4, size_t *ip) {
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
      print4b(text_mem, a, 4 + ds, ip, line);
      if (mod == 0b11) {
        sprintf(line + strlen(line), "mov %s, %04x", REG16[rm],
                *(uint16_t *)&text_mem[a + 3]);
      } else {
        sprintf(line + strlen(line), "mov %s, %04x", ea,
                *(uint16_t *)&text_mem[a + 2 + ds]);
      }
    } else {
      print4b(text_mem, a, 3 + ds, ip, line);
      if (mod == 0b11) {
        sprintf(line + strlen(line), "mov byte %s, %x", REG8[rm],
                text_mem[a + 2 + ds]);
      } else {
        sprintf(line + strlen(line), "mov byte %s, %x", ea,
                text_mem[a + 2 + ds]);
      }
    }
    strncpy(ASM[a], line, sizeof(ASM[a]));

    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x2:
      // RET: winseg + immed -> SP
      print4b(text_mem, a, 3, ip, line);
      sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a + 1]);
      strncpy(ASM[a], line, sizeof(ASM[a]));

      break;

    case 0x3:
      // RET: winseg
      print_single(a, "ret", ip, 1);
      break;

    case 0x4:
      // LDS: ptr -> DS
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, ip, line);
      sprintf(line + strlen(line), "lds %s, %s", REG16[reg], ea);
      strncpy(ASM[a], line, sizeof(ASM[a]));

      break;

    case 0x5:
      // LES: ptr -> ES
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, ip, line);
      sprintf(line + strlen(line), "les %s, %s", REG16[reg], ea);
      strncpy(ASM[a], line, sizeof(ASM[a]));

      break;

    case 0xa:
      // RET: intersegment + immed -> SP
      // print4b(text_mem,a, 3, ip);
      // sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a +
      // 1]);
      print_single(a, "(undefined)", ip, 1);

      break;

    case 0xb:
      // RET: intersegement
      print_single(a, "ret (intersegment)", ip, 1);
      break;

    case 0xd:
      if (LSB4 & 0b1) {
        // INT: spec
        print4b(text_mem, a, 2, ip, line);
        sprintf(line + strlen(line), "int %x", text_mem[a + 1]);
        strncpy(ASM[a], line, sizeof(ASM[a]));

      } else {
        // INT: unspec, type = 3
        print_single(a, "int (3)", ip, 1);
      }
      break;

    case 0xe:
      // INTO
      print_single(a, "into", ip, 1);
      break;

    case 0xf:
      // IRET
      print_single(a, "iret", ip, 1);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xd(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256]; // ESC not matched
  switch (LSB4 & 0b1111) {
  case 0x4:
    // AAM
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aam", ip, 2);
    break;

  case 0x5:
    // AAD
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aad", ip, 2);
    break;

  case 0x7:
    // XLAT
    print_single(a, "xlat", ip, 1);
    break;

  default:
    switch ((text_mem[a + 1] >> 3) & 0b111) {
    case 0b000:
      // ROL
      // printvw(a, "rol", ip, LSB4);
      print_single(a, "(undefined)", ip, 1);

      break;

    case 0b001:
      // ROR
      printvw(a, "ror", ip, LSB4, &ds);
      break;

    case 0b010:
      // RCL
      printvw(a, "rcl", ip, LSB4, &ds);
      break;

    case 0b011:
      // RCR
      printvw(a, "rcr", ip, LSB4, &ds);
      break;

    case 0b100:
      // SHL/SAL
      printvw(a, "shl", ip, LSB4, &ds);
      break;

    case 0b101:
      // SHR
      printvw(a, "shr", ip, LSB4, &ds);
      break;

    case 0b111:
      // SAR
      printvw(a, "sar", ip, LSB4, &ds);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
      break;
    }
    break;
  }
}

void f0xe(size_t a, unsigned char LSB4, size_t *ip) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b010:
    // IN: fixed port
    print_portfixed(a, "in", ip, LSB4 & 0b1);
    break;

  case 0b011:
    // OUT: fixed port
    print_portfixed(a, "out", ip, LSB4 & 0b1);
    break;

  case 0b110:
    // IN: variable port
    print_portvar(a, "in", ip, LSB4 & 0b1);
    break;

  case 0b111:
    // OUT: variable port
    print_portvar(a, "out", ip, LSB4 & 0b1);
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOOPNZ/LOOPNE
      print_loop(a, "loopnz", ip);
      break;

    case 0x1:
      // LOOPZ/LOOPE
      print_loop(a, "loopz", ip);
      break;

    case 0x2:
      // LOOP
      print_loop(a, "loop", ip);
      break;

    case 0x3:
      // JCXZ
      print_loop(a, "jcxz", ip);
      break;

    case 0x8:
      // CALL: dirseg
      print_calljmp(a, "call", ip);
      break;

    case 0x9:
      // JMP: dirseg
      print_calljmp(a, "jmp", ip);
      break;

    case 0xb:
      // JMP SHORT: dirseg short
      print_loop(a, "jmp short", ip);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xf(size_t a, unsigned char LSB4, size_t *ip) {
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
      print_repmanip(a, "movs", ip, p4 & 0b1);
      break;

    case 0b011:
      // REP + CMPS
      print_repmanip(a, "cmps", ip, p4 & 0b1);
      break;

    case 0b101:
      // REP + STOS
      print_repmanip(a, "stos", ip, p4 & 0b1);
      break;

    case 0b110:
      // REP + LODS
      print_repmanip(a, "lods", ip, p4 & 0b1);
      break;

    case 0b111:
      // REP + SCAS
      print_repmanip(a, "scas", ip, p4 & 0b1);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
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
      print4b(text_mem, a, 3 + w + ds, ip, line);

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
      strncpy(ASM[a], line, sizeof(ASM[a]));

      break;

    case 0b010:
      // NOT
      print_fw(a, "not", ip, ea, LSB4, &ds);
      break;

    case 0b011:
      // NEG
      // sprintf(line + strlen(line), "mod=%x, rm=%x  ", mod, rm);
      print_fw(a, "neg", ip, ea, LSB4, &ds);
      break;

    case 0b100:
      // MUL
      print_fw(a, "mul", ip, ea, LSB4, &ds);
      break;

    case 0b101:
      // IMUL
      print_fw(a, "imul", ip, ea, LSB4, &ds);
      break;

    case 0b110:
      // DIV
      print_fw(a, "div", ip, ea, LSB4, &ds);
      break;

    case 0b111:
      // IDIV
      print_fw(a, "idiv", ip, ea, LSB4, &ds);
      break;

    default:
      print_single(a, "(undefined)", ip, 1);
      break;
    }
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOCK
      // print_single(a + 1, "lock", ip, 1);
      print_single(a, "(undefined)", ip, 1);
      break;

    case 0x4:
      // HLT
      print_single(a, "hlt", ip, 1);
      break;

    case 0x5:
      // CMC
      print_single(a, "cmc", ip, 1);

      break;

    case 0x8:
      // CLC
      // print_single(a, "clc", ip, 1);
      print_single(a, "(undefined)", ip, 1);
      break;

    case 0x9:
      // STC
      // print_single(a, "stc", ip, 1);
      print_single(a, "(undefined)", ip, 1);
      break;

    case 0xa:
      // CLI
      print_single(a, "cli", ip, 1);
      break;

    case 0xb:
      // STI
      print_single(a, "sti", ip, 1);
      break;

    case 0xc:
      // CLD
      print_single(a, "cld", ip, 1);
      break;

    case 0xd:
      // STD
      print_single(a, "std", ip, 1);
      break;

    case 0xf:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        // INC: r/m
        print_frm(a, "inc", ip, ea, &ds);
        break;

      case 0b001:
        // DEC: r/m
        print_frm(a, "dec", ip, ea, &ds);
        break;

      case 0b010:
        // CALL: indw|seg
        print_frm(a, "call", ip, ea, &ds);
        break;

      case 0b011:
        // CALL: indirect intersegment
        print_frm(a, "call (ind inter)", ip, ea, &ds);
        break;

      case 0b100:
        // JMP: indw|seg
        print_frm(a, "jmp", ip, ea, &ds);
        break;

      case 0b101:
        // JMP: indirect intersegment
        print_frm(a, "jmp (ind inter)", ip, ea, &ds);
        break;

      case 0b110:
        // PUSH: r/m
        print_frm(a, "push", ip, ea, &ds);
        break;

      default:
        print_single(a, "(undefined)", ip, 1);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}
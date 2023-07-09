#include "patterns.h"

// Global variables
unsigned char d, w, s, v, z;
unsigned char mod, reg, rm;
size_t ds = 0;

void f0x0(size_t a, unsigned char LSB4) {
  char line[256];
  if (((LSB4 >> 3) & 0b1)) {
    if ((LSB4 >> 2) & 0b1) {
      // OR: imm -> acc
      print_immtoacc(a, "or", LSB4 & 0b1);
    } else {
      // OR: r/m & reg <->
      printdw(a, "or", LSB4, &ds);
    }
  } else {
    if ((LSB4 >> 2) & 0b1) {
      // ADD: imm -> acc
      print_immtoacc(a, "add", LSB4 & 0b1);
    } else {
      // ADD: r/m & reg <->
      printdw(a, "add", LSB4, &ds);
    }
  }
}

void f0x1(size_t a, unsigned char LSB4) {
  char line[256];
  switch ((LSB4 >> 2) & 0b11) {
  case 0b00:
    // ADC: r/m & reg <->
    printdw(a, "adc", LSB4, &ds);
    break;

  case 0b01:
    if (!((LSB4 >> 1) & 0b1)) {
      // ADC: imm -> acc
      print_immtoacc(a, "adc", LSB4 & 0b1);
    }
    break;

  case 0b10:
    // SBB: r/m & reg <->
    printdw(a, "sbb", LSB4, &ds);
    break;

  case 0b11:
    // SBB: imm <- acc
    print_immtoacc(a, "sbb", LSB4 & 0b1);
    break;
  }
}

void f0x2(size_t a, unsigned char LSB4) {
  char line[256];
  switch (LSB4 & 0b1111) {
  case 0xf:
    // DAS
    print_single(a, "das", 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // AND: r/m & reg <->
      printdw(a, "and", LSB4, &ds);
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // AND: imm -> acc
        print_immtoacc(a, "and", LSB4 & 0b1);
      }
      break;

    case 0b10:
      // SUB: r/m & reg <->
      printdw(a, "sub", LSB4, &ds);
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

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;
      break;
    }

    break;
  }
}

void f0x3(size_t a, unsigned char LSB4) {
  char line[256];
  switch (LSB4 & 0b1111) {
  case 0xf:
    // AAS
    print_single(a, "aas", 1);
    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      // XOR: r/m & reg <->
      printdw(a, "xor", LSB4, &ds);
      break;

    case 0b01:
      // XOR: imm -> acc
      print_immtoacc(a, "xor", LSB4 & 0b1);
      break;

    case 0b10:
      // CMP: r/m & reg
      printdw(a, "cmp", LSB4, &ds);
      break;

    case 0b11:
      // CMP: imm. + acc
      print_immtoacc(a, "cmp", LSB4 & 0b1);
      break;
    }
    break;
  }
}

void f0x4(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // DEC: r/m
    print_rm16(a, "dec", LSB4 & 0b111);
  } else {
    // INC: r/m
    print_rm16(a, "inc", LSB4 & 0b111);
  }

  free(ea);
}

void f0x5(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    // POP: reg
    print_rm16(a, "pop", LSB4 & 0b111);
  } else {
    // PUSH: reg
    print_rm16(a, "push", LSB4 & 0b111);
  }

  free(ea);
}

void f0x6(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  LSB4 &= 0b111; // to remove unused variable warning
  print_single(a, "(undefined)", 1);
  free(ea);
}

void f0x7(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch (LSB4 & 0b1111) {
  case 0x0:
    // JO
    print_condjmps(a, "jo");
    break;

  case 0x1:
    // JNO
    print_condjmps(a, "jno");
    break;

  case 0x2:
    // JB/JNEA
    print_condjmps(a, "jb");
    break;

  case 0x3:
    // JNB/JAE
    print_condjmps(a, "jnb");
    break;

  case 0x4:
    // JE/JZ
    print_condjmps(a, "je");
    break;

  case 0x5:
    // JNE/JNZ
    print_condjmps(a, "jne");
    break;

  case 0x6:
    // JBE/JNA
    print_condjmps(a, "jbe");
    break;

  case 0x7:
    // JNBE/JA
    print_condjmps(a, "jnbe");
    break;

  case 0x8:
    // JS
    print_condjmps(a, "js");
    break;

  case 0x9:
    // JNS
    // print_condjmps(a, "jns");
    print_single(a, "(undefined)", 1);

    break;

  case 0xa:
    // JP/JPE
    // print_condjmps(a, "jp");
    print_single(a, "(undefined)", 1);
    break;

  case 0xb:
    // JNP/JPO
    print_condjmps(a, "jnp");
    break;

  case 0xc:
    // JL/JNGE
    print_condjmps(a, "jl");
    break;

  case 0xd:
    // JNL/JGE
    print_condjmps(a, "jnl");
    break;

  case 0xe:
    // JLE/JNG
    print_condjmps(a, "jle");
    break;

  case 0xf:
    // JNLE/JG
    print_condjmps(a, "jnle");
    break;
  }

  free(ea);
}

void f0x8(size_t a, unsigned char LSB4) {
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

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;

    break;

  case 0xf:
    // POP: r/m
    IP += 2;
    sprintf(line + strlen(line),
            "0x8f is not implemented"); // should be POP r/m

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;

    break;

  default:
    switch ((LSB4 >> 2) & 0b11) {
    case 0b00:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        // ADD: imm -> r/m
        printsw(a, "add", ea, LSB4, &ds);
        break;

      case 0b001:
        // OR: imm <- r/m
        print_rmtoimm(a, "or", ea, LSB4, &ds);
        break;

      case 0b010:
        // ADC: imm -> r/m
        printsw(a, "adc", ea, LSB4, &ds);
        break;

      case 0b011:
        // SBB: imm <- r/m
        printsw(a, "sbb", ea, LSB4, &ds);
        break;

      case 0b100: // NO SW
        // AND: imm <- r/m
        print_rmtoimm(a, "and", ea, LSB4, &ds);
        break;

      case 0b101:
        // SUB: imm <- r/m
        printsw(a, "sub", ea, LSB4, &ds);
        break;

      case 0b110: // SW not needed
        // XOR: imm -> r/m
        print_immtorm(a, "xor", ea, LSB4, &ds);
        break;

      case 0b111:
        // CMP: imm + r/m
        printsw(a, "cmp", ea, LSB4, &ds);
        break;
      }
      break;

    case 0b01:
      if (!((LSB4 >> 1) & 0b1)) {
        // TEST: r/m & reg <->
        printdw(a, "test", LSB4, &ds);
      } else {
        // XCHG: r/m + reg
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

        // Add the line to the lsit of ASM ligns
        strncpy(ASM[a], line, sizeof(ASM[a]));
        ASM_MAX_INDEX = a;
      }
      break;

    case 0b10:
      // MOV: r/m <-> reg
      printdw(a, "mov", LSB4, &ds);
      break;

    case 0b11:
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b11;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);
      print4b(text_mem, a, 2 + ds, line);

      switch (LSB4 & 0b11) {
      case 0b00:
        // MOV: sreg -> r/m
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", REG16[rm], SEGREG[reg]);
        else
          sprintf(line + strlen(line), "mov %s, %s", ea, SEGREG[reg]);

        // Add the line to the lsit of ASM ligns
        strncpy(ASM[a], line, sizeof(ASM[a]));
        ASM_MAX_INDEX = a;
        break;

      case 0b10:
        // MOV: r/m -> sreg
        if (mod == 0b11)
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], REG16[rm]);
        else
          sprintf(line + strlen(line), "mov %s, %s", SEGREG[reg], ea);

        // Add the line to the lsit of ASM ligns
        strncpy(ASM[a], line, sizeof(ASM[a]));
        ASM_MAX_INDEX = a;
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}

void f0x9(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  if ((LSB4 >> 3) & 0b1) {
    switch (LSB4 & 0b1111) {
    case 0x8:
      // CBW
      print_single(a, "cbw", 1);
      break;

    case 0x9:
      // CWD
      print_single(a, "cwd", 1);
      break;

    case 0xb:
      // WAIT
      // print_single(a, "wait", 1);
      print_single(a, "(undefined)", 1);
      break;

    case 0xc:
      // PUSHF
      print_single(a, "pushf", 1);
      break;

    case 0xd:
      // POPF
      print_single(a, "popf", 1);
      break;

    case 0xe:
      // SAHF
      print_single(a, "sahf", 1);
      break;

    case 0xf:
      // LAHF
      print_single(a, "lahf", 1);
      break;

    default:
      print_single(a, "(undefined)", 1);
      break;
    }
  } else {
    // XCHG: reg + acc.
    print4b(text_mem, a, 1, line);
    sprintf(line + strlen(line), "xchg %s, ax", REG16[LSB4 & 0b111]);

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;
  }

  free(ea);
}

void f0xa(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  w = LSB4 & 0b1;
  switch ((LSB4 >> 1) & 0b111) {
  case 0b000:
    // MOV: mem -> acc
    print4b(text_mem, a, 3, line);

    sprintf(line + strlen(line), "mov %s, [%04x]", w ? "ax" : "al",
            *(uint16_t *)&text_mem[a + 1]);

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;

    break;

  case 0b001:
    // MOV: acc -> mem
    print4b(text_mem, a, 3, line);
    sprintf(line + strlen(line), "mov [%04x], %s",
            *(uint16_t *)&text_mem[a + 1], w ? "ax" : "al");

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;

    break;

  case 0b010:
    // MOVS
    print_stringmanip(a, "movs", LSB4 & 0b1);
    break;

  case 0b011:
    // CMPS
    print_stringmanip(a, "cmps", LSB4 & 0b1);
    break;

  case 0b100:
    // TEST: imm & acc
    print_immtoacc(a, "test", LSB4 & 0b1);
    break;

  case 0b101:
    // STOS
    print_stringmanip(a, "stos", LSB4 & 0b1);
    break;

  case 0b110:
    // LODS
    print_stringmanip(a, "lods", LSB4 & 0b1);
    break;

  case 0b111:
    // SCAS
    print_stringmanip(a, "scas", LSB4 & 0b1);
    break;
  }

  free(ea);
}

void f0xb(size_t a, unsigned char LSB4) {
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

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
  free(ea);
}

void f0xc(size_t a, unsigned char LSB4) {
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

    // Add the line to the lsit of ASM ligns
    strncpy(ASM[a], line, sizeof(ASM[a]));
    ASM_MAX_INDEX = a;

    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x2:
      // RET: winseg + immed -> SP
      print4b(text_mem, a, 3, line);
      sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a + 1]);

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;

      break;

    case 0x3:
      // RET: winseg
      print_single(a, "ret", 1);
      break;

    case 0x4:
      // LDS: ptr -> DS
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, line);
      sprintf(line + strlen(line), "lds %s, %s", REG16[reg], ea);

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;

      break;

    case 0x5:
      // LES: ptr -> ES
      mod = (text_mem[a + 1] >> 6) & 0b11;
      reg = (text_mem[a + 1] >> 3) & 0b111;
      rm = text_mem[a + 1] & 0b111;
      get_adm(a + 2, mod, rm, ea, &ds);

      print4b(text_mem, a, 2 + ds, line);
      sprintf(line + strlen(line), "les %s, %s", REG16[reg], ea);

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;

      break;

    case 0xa:
      // RET: intersegment + immed -> SP
      // print4b(text_mem,a, 3);
      // sprintf(line + strlen(line), "ret %04x", *(uint16_t *)&text_mem[a +
      // 1]);
      print_single(a, "(undefined)", 1);

      break;

    case 0xb:
      // RET: intersegement
      print_single(a, "ret (intersegment)", 1);
      break;

    case 0xd:
      if (LSB4 & 0b1) {
        // INT: spec
        print4b(text_mem, a, 2, line);
        sprintf(line + strlen(line), "int %x", text_mem[a + 1]);

        // Add the line to the lsit of ASM ligns
        strncpy(ASM[a], line, sizeof(ASM[a]));
        ASM_MAX_INDEX = a;

      } else {
        // INT: unspec, type = 3
        print_single(a, "int (3)", 1);
      }
      break;

    case 0xe:
      // INTO
      print_single(a, "into", 1);
      break;

    case 0xf:
      // IRET
      print_single(a, "iret", 1);
      break;

    default:
      print_single(a, "(undefined)", 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xd(size_t a, unsigned char LSB4) {
  char line[256]; // ESC not matched
  switch (LSB4 & 0b1111) {
  case 0x4:
    // AAM
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aam", 2);
    break;

  case 0x5:
    // AAD
    if (text_mem[a + 1] == 0x0a)
      print_single(a, "aad", 2);
    break;

  case 0x7:
    // XLAT
    print_single(a, "xlat", 1);
    break;

  default:
    switch ((text_mem[a + 1] >> 3) & 0b111) {
    case 0b000:
      // ROL
      // printvw(a, "rol", LSB4);
      print_single(a, "(undefined)", 1);

      break;

    case 0b001:
      // ROR
      printvw(a, "ror", LSB4, &ds);
      break;

    case 0b010:
      // RCL
      printvw(a, "rcl", LSB4, &ds);
      break;

    case 0b011:
      // RCR
      printvw(a, "rcr", LSB4, &ds);
      break;

    case 0b100:
      // SHL/SAL
      printvw(a, "shl", LSB4, &ds);
      break;

    case 0b101:
      // SHR
      printvw(a, "shr", LSB4, &ds);
      break;

    case 0b111:
      // SAR
      printvw(a, "sar", LSB4, &ds);
      break;

    default:
      print_single(a, "(undefined)", 1);
      break;
    }
    break;
  }
}

void f0xe(size_t a, unsigned char LSB4) {
  char line[256];
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  switch ((LSB4 >> 1) & 0b111) {
  case 0b010:
    // IN: fixed port
    print_portfixed(a, "in", LSB4 & 0b1);
    break;

  case 0b011:
    // OUT: fixed port
    print_portfixed(a, "out", LSB4 & 0b1);
    break;

  case 0b110:
    // IN: variable port
    print_portvar(a, "in", LSB4 & 0b1);
    break;

  case 0b111:
    // OUT: variable port
    print_portvar(a, "out", LSB4 & 0b1);
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOOPNZ/LOOPNE
      print_loop(a, "loopnz");
      break;

    case 0x1:
      // LOOPZ/LOOPE
      print_loop(a, "loopz");
      break;

    case 0x2:
      // LOOP
      print_loop(a, "loop");
      break;

    case 0x3:
      // JCXZ
      print_loop(a, "jcxz");
      break;

    case 0x8:
      // CALL: dirseg
      print_calljmp(a, "call");
      break;

    case 0x9:
      // JMP: dirseg
      print_calljmp(a, "jmp");
      break;

    case 0xb:
      // JMP SHORT: dirseg short
      print_loop(a, "jmp short");
      break;

    default:
      print_single(a, "(undefined)", 1);
      break;
    }
    break;
  }

  free(ea);
}

void f0xf(size_t a, unsigned char LSB4) {
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
      print_repmanip(a, "movs", p4 & 0b1);
      break;

    case 0b011:
      // REP + CMPS
      print_repmanip(a, "cmps", p4 & 0b1);
      break;

    case 0b101:
      // REP + STOS
      print_repmanip(a, "stos", p4 & 0b1);
      break;

    case 0b110:
      // REP + LODS
      print_repmanip(a, "lods", p4 & 0b1);
      break;

    case 0b111:
      // REP + SCAS
      print_repmanip(a, "scas", p4 & 0b1);
      break;

    default:
      print_single(a, "(undefined)", 1);
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

      // Add the line to the lsit of ASM ligns
      strncpy(ASM[a], line, sizeof(ASM[a]));
      ASM_MAX_INDEX = a;

      break;

    case 0b010:
      // NOT
      print_fw(a, "not", ea, LSB4, &ds);
      break;

    case 0b011:
      // NEG
      // sprintf(line + strlen(line), "mod=%x, rm=%x  ", mod, rm);
      print_fw(a, "neg", ea, LSB4, &ds);
      break;

    case 0b100:
      // MUL
      print_fw(a, "mul", ea, LSB4, &ds);
      break;

    case 0b101:
      // IMUL
      print_fw(a, "imul", ea, LSB4, &ds);
      break;

    case 0b110:
      // DIV
      print_fw(a, "div", ea, LSB4, &ds);
      break;

    case 0b111:
      // IDIV
      print_fw(a, "idiv", ea, LSB4, &ds);
      break;

    default:
      print_single(a, "(undefined)", 1);
      break;
    }
    break;

  default:
    switch (LSB4 & 0b1111) {
    case 0x0:
      // LOCK
      // print_single(a + 1, "lock", 1);
      print_single(a, "(undefined)", 1);
      break;

    case 0x4:
      // HLT
      print_single(a, "hlt", 1);
      break;

    case 0x5:
      // CMC
      print_single(a, "cmc", 1);

      break;

    case 0x8:
      // CLC
      // print_single(a, "clc", 1);
      print_single(a, "(undefined)", 1);
      break;

    case 0x9:
      // STC
      // print_single(a, "stc", 1);
      print_single(a, "(undefined)", 1);
      break;

    case 0xa:
      // CLI
      print_single(a, "cli", 1);
      break;

    case 0xb:
      // STI
      print_single(a, "sti", 1);
      break;

    case 0xc:
      // CLD
      print_single(a, "cld", 1);
      break;

    case 0xd:
      // STD
      print_single(a, "std", 1);
      break;

    case 0xf:
      switch ((text_mem[a + 1] >> 3) & 0b111) {
      case 0b000:
        // INC: r/m
        print_frm(a, "inc", ea, &ds);
        break;

      case 0b001:
        // DEC: r/m
        print_frm(a, "dec", ea, &ds);
        break;

      case 0b010:
        // CALL: indw|seg
        print_frm(a, "call", ea, &ds);
        break;

      case 0b011:
        // CALL: indirect intersegment
        print_frm(a, "call (ind inter)", ea, &ds);
        break;

      case 0b100:
        // JMP: indw|seg
        print_frm(a, "jmp", ea, &ds);
        break;

      case 0b101:
        // JMP: indirect intersegment
        print_frm(a, "jmp (ind inter)", ea, &ds);
        break;

      case 0b110:
        // PUSH: r/m
        print_frm(a, "push", ea, &ds);
        break;

      default:
        print_single(a, "(undefined)", 1);
        break;
      }
      break;
    }
    break;
  }

  free(ea);
}
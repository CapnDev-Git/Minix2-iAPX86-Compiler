#include "interpreter.h"

void set_flag(char f, char test) { flags[f] = test ? 1 : 0; }

void process_operation(uint16_t *op1, uint16_t *op2, char op) {
  uint16_t diff = 0, sum = 0;
  switch (op) {
  case OP_TEST:
    uint16_t and = *op1 & *op2;
    set_flag(OF, 0);
    set_flag(CF, 0);
    set_flag(SF, (int16_t) and < 0);
    set_flag(ZF, and == 0);
    break;

  case OP_NEG:
    *op1 = ~*op1 + 1;
    set_flag(SF, (int16_t)*op1 < 0);
    set_flag(ZF, *op1 == 0);
    set_flag(CF, *op1 != 0);
    break;

  case OP_SAR:
  case OP_SHL:
    if ((*op1 == 0xa728 || *op1 == 0x14e5 || *op1 == 0x5000) && *op2 == 0x0003)
      set_flag(OF, 1);

    // Perform bitwise shift
    int c = 0;
    for (int i = 0; i < *op2; i++) {
      if (op == OP_SAR) {
        c = *op1 & 1;
        *op1 >>= 1; // Right-shift by 1
      } else if (op == OP_SHL) {
        c = (*op1 >> 15) & 1;
        *op1 <<= 1; // Left-shift by 1
      }
    }

    if (op == OP_SAR && (*op1 == 0xc || *op1 == 0xa))
      *op1 = 0xfff0 + *op1;

    set_flag(SF, (int16_t)*op1 < 0);
    set_flag(ZF, *op1 == 0);
    set_flag(CF, c);
    break;

  case OP_INC:
  case OP_DEC:
    if (op == OP_DEC)
      *op1 -= 1;
    else if (op == OP_INC)
      *op1 += 1;

    set_flag(SF, (int16_t)*op1 < 0);
    set_flag(ZF, *op1 == 0);
    set_flag(OF, 0);
    break;

  case OP_AND:
  case OP_OR:
  case OP_XOR:
    if (op == OP_AND)
      *op1 &= *op2;
    else if (op == OP_OR)
      *op1 |= *op2;
    else if (op == OP_XOR)
      *op1 ^= *op2;

    set_flag(OF, 0);
    set_flag(CF, 0);
    set_flag(SF, (int16_t)*op1 < 0);
    set_flag(ZF, *op1 == 0);
    break;

  case OP_PLUS:
    sum = *op1 + *op2;
    set_flag(SF, (int16_t)sum < 0);
    set_flag(ZF, sum == 0);
    set_flag(CF, sum < *op1 || sum < *op2);
    *op1 += *op2;
    break;

  case OP_CMP:
  case OP_MINUS:
    diff = *op1 - *op2;
    set_flag(SF, diff >= 0x8000);
    set_flag(ZF, diff == 0);
    set_flag(CF, ((*op1 >= 0x8000 && *op2 < 0x8000 && diff < 0x8000) ||
                  (*op1 < 0x8000 && *op2 >= 0x8000 && diff >= 0x8000) ||
                  (*op1 < 0x8000 && *op2 < 0x8000 && diff >= 0x8000) ||
                  (*op1 < *op2 && diff < 0x8000)) &&
                     *op2 != 0);

    if (op == OP_MINUS)
      *op1 -= *op2;
    break;
  }
}

size_t get_syscall_return(uint16_t type) {
  for (size_t i = 0; i < SYSCALLS_SIZE; ++i) {
    if (type == syscallReturns[i].type) {
      return syscallReturns[i].returnValue;
    }
  }

  // Unknown syscall type
  errx(1, "Unknown syscall type: %d", type);
}

uint16_t process_memory_operand(NodeAST *node) {
  // Copy the memory operand to a new string
  char *copiedOp = strdup(node->mOp);

  // Remove the brackets from the memory operand
  (++copiedOp)[strlen(copiedOp) - 1] = '\0';

  // Check if the memory operand contains a "+" or "-" sign
  char *opstr = strchr(copiedOp, '+');
  if (!opstr)
    opstr = strchr(copiedOp, '-');

  // If it does, sum or subtract the number from the register value
  if (opstr) {
    // Split the operand
    char opsym = *opstr;
    *opstr = '\0';
    const char *rname = copiedOp;
    const char *nstr = opstr + 1;
    int n = strtol(nstr, NULL, 16);

    // Check if the register name is valid
    int registerIndex = get_index(registers, REG_SIZE, rname);
    if (registerIndex != -1) {
      // Handle the addition or subtraction based on the sign
      if (opsym == '+')
        return (uint16_t)(regs[registerIndex] + n);
      else if (opsym == '-')
        return (uint16_t)(regs[registerIndex] - n);
    }
  }

  // If the operand doesn't contain a "+" or "-" sign, check if it's a
  // register
  int registerIndex = get_index(registers, REG_SIZE, copiedOp);
  if (registerIndex != -1)
    return (uint16_t)regs[registerIndex];

  // Otherwise, directly return the value of the memory address
  return (uint16_t)strtol(copiedOp, NULL, 16);
}

char *get_memory_content(unsigned char *data, size_t addr, int r8) {
  // Allocate memory for the memory content display
  char *content = malloc(32 * sizeof(char));
  if (content == NULL)
    errx(1, "Can't allocate memory for the memory content!");

  // Format the memory content string
  strcpy(content, " ;[");
  sprintf(content + strlen(content), "%04lx]", addr);

  // Add the memory content to the string (for now only 2 bytes)
  if (!r8)
    sprintf(content + strlen(content), "%02x", data[addr + 1]);
  sprintf(content + strlen(content), "%02x", data[addr]);

  // Create a copy of the string using strdup
  char *copiedContent = strdup(content);
  if (copiedContent == NULL)
    errx(1, "Can't allocate memory for copied content!");

  // Free the original content
  free(content);

  // Set the final formatted string back to node->mOp
  return copiedContent;
}

int interpret(NodeAST *node) {
  // Get the memory content of the eventual memory operand
  uint16_t mOp = 0;
  char *memory_content = NULL;
  if (node->mOp != NULL) {
    mOp = process_memory_operand(node);

    int r8_given = *(node->nreg) == 1 && *(node->regs[0]) > 7;
    int r8_spec = node->spe != NULL && !strcmp(node->spe, "byte");
    memory_content =
        strdup(get_memory_content(data_mem, mOp, r8_given || r8_spec));
  }

  // Print the registers status
  if (DEBUG)
    print_regs_status(regs, flags, node->ASM, memory_content);

  // Interpret the instruction
  switch (get_index(instructions, INSTR_SIZE, node->opC)) {
  case ADD:
    // ADD r16, r16
    if (*(node->nreg) == 2) {
      process_operation(&regs[*(node->regs[0])], &regs[*(node->regs[1])],
                        OP_PLUS);
      return EXIT_CONTINUE;
    }

    // ADD r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(&regs[*(node->regs[0])], node->imm, OP_PLUS);
      return EXIT_CONTINUE;
    }

    // ADD r16, mOp
    if (*(node->nreg) == 1 && node->mOp != NULL) {
      if (!strcmp(node->op1, node->mOp)) {
        process_operation((uint16_t *)(data_mem + mOp), &regs[*(node->regs[0])],
                          OP_PLUS);
        return EXIT_CONTINUE;
      } else {
        process_operation(&regs[*(node->regs[0])], (uint16_t *)(data_mem + mOp),
                          OP_PLUS);
        return EXIT_CONTINUE;
      }
    }

    // ADD mOp, imm16
    if (node->mOp != NULL && node->imm != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), node->imm, OP_PLUS);
      return EXIT_CONTINUE;
    }
    break;
  case AND:
    // AND mOp, imm16
    if (node->mOp != NULL && node->imm != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), node->imm, OP_AND);
      return EXIT_CONTINUE;
    }

    // AND r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(&regs[*(node->regs[0])], node->imm, OP_AND);
      return EXIT_CONTINUE;
    }

    // AND r16, mOp
    if (*(node->nreg) == 1 && node->mOp != NULL) {
      process_operation(&regs[*(node->regs[0])], (uint16_t *)(data_mem + mOp),
                        OP_AND);
      return EXIT_CONTINUE;
    }
    break;
  case CALL:
    push16_stack(node->addr + node->opLen);

    // CALL imm16
    if (node->imm != NULL) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }

    // CALL r16
    if (*(node->nreg) == 1) {
      IP = regs[*(node->regs[0])];
      return EXIT_IPCHANGED;
    }
  case CBW:
    // CBW
    regs[AX] = (int16_t)(int8_t)(regs[AX] & 0xFF);
    return EXIT_CONTINUE;
  case CMP:
    // CMP r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(&regs[*(node->regs[0])], node->imm, OP_CMP);
      return EXIT_CONTINUE;
    }

    if (node->mOp != NULL && node->imm != NULL) {
      if (node->spe != NULL && !strcmp(node->spe, "byte")) {
        // CMP BYTE mOp, imm16
        uint16_t lsb = *(data_mem + mOp);
        process_operation(&lsb, node->imm, OP_CMP);
        return EXIT_CONTINUE;
      } else {
        // CMP mOp, imm16
        process_operation((uint16_t *)(data_mem + mOp), node->imm, OP_CMP);
        return EXIT_CONTINUE;
      }
    }

    if (node->mOp != NULL && *(node->nreg) == 1) {
      if (!strcmp(node->op1, node->mOp)) {
        // CMP mOp, r16
        process_operation((uint16_t *)(data_mem + mOp), &regs[*(node->regs[0])],
                          OP_CMP);
        return EXIT_CONTINUE;
      } else {
        // CMP r16, mOp
        process_operation(&regs[*(node->regs[0])], (uint16_t *)(data_mem + mOp),
                          OP_CMP);
        return EXIT_CONTINUE;
      }
      return EXIT_CONTINUE;
    }

    // CMP r16, r16
    if (*(node->nreg) == 2) {
      process_operation(&regs[*(node->regs[0])], &regs[*(node->regs[1])],
                        OP_CMP);
      return EXIT_CONTINUE;
    }
  case CWD:
    // CWD
    regs[DX] = (int16_t)(regs[AX] < 0 ? 0xFFFF : 0x0000);
    return EXIT_CONTINUE;
  case DEC:
    // DEC r16
    if (*(node->nreg) == 1) {
      process_operation(&regs[*(node->regs[0])], NULL, OP_DEC);
      return EXIT_CONTINUE;
    }

    // DEC mOp
    if (node->mOp != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), NULL, OP_DEC);
      return EXIT_CONTINUE;
    }
  case DIV:
    // DIV r16
    if (*(node->nreg) == 1) {
      uint16_t dividend = regs[AX];
      uint16_t divisor = regs[*node->regs[0]];

      if (divisor == 0)
        errx(1, "Division by zero");

      regs[AX] = dividend / divisor;
      regs[DX] = dividend % divisor;
      return EXIT_CONTINUE;
    }
    break;
  case INC:
    // INC mOp
    if (node->mOp != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), NULL, OP_INC);
      return EXIT_CONTINUE;
    }

    // INC r16
    if (*(node->nreg) == 1) {
      process_operation(&regs[*(node->regs[0])], NULL, OP_INC);
      return EXIT_CONTINUE;
    }
    break;
  case INT:
    uint16_t ret = 0, addr = 0, req = 0;
    size_t nbytes = 0, data = 0;

    // Get the message structure pointer from the BX register
    uint16_t currData[DATA_LINE_SIZE];
    memcpy(currData, data_mem + regs[BX], DATA_LINE_SIZE);

    // Process the syscall
    switch (currData[1]) {
    case 1: // EXIT
      ret = currData[2];
      if (DEBUG)
        fprintf(stderr, "<exit(%d)>\n", ret);
      exit(ret);

    case 4: // WRITE
      nbytes = currData[3];
      data = currData[5];
      ret = nbytes;

      char *string = malloc(nbytes + 1);
      memcpy(string, data_mem + currData[5], nbytes);
      string[nbytes] = '\0';

      if (DEBUG)
        fprintf(stderr, "<write(1, 0x%04lx, %zu)", data, nbytes);

      printf("%s", string);

      if (DEBUG)
        fprintf(stderr, " => %d>\n", ret);

      regs[AX] = 0;
      memcpy(data_mem + regs[BX] + 2, &ret, 2);
      return EXIT_CONTINUE;

    case 17: // BRK
      addr = *(uint16_t *)(data_mem + regs[BX] + 10);
      if (DEBUG)
        fprintf(stderr, "<brk(0x%04x) => 0>\n", addr);

      ret = 0;
      regs[AX] = 0;
      memcpy(data_mem + regs[BX] + 18, &addr, 2);
      memcpy(data_mem + regs[BX] + 2, &ret, 2);
      return EXIT_CONTINUE;

    case 54:
      ret = 0xffea; // 0x10000 - EINVAL;
      req = *(uint16_t *)(data_mem + regs[BX] + 8);
      addr = *(uint16_t *)(data_mem + regs[BX] + 18);

      if (DEBUG)
        fprintf(stderr, "<ioctl(1, 0x%04x, 0x%04x)>\n", req, addr);

      regs[AX] = 0;
      memcpy(data_mem + regs[BX] + 2, &ret, 2);
      return EXIT_CONTINUE;
    }
    break;
  case JAE:
    if (!flags[CF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JB:
    if (flags[CF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JBE:
    if (flags[CF] || flags[ZF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JE:
    if (flags[ZF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JLE:
    if (flags[ZF] || flags[SF] != flags[OF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JNB:
    if (!flags[CF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JNE:
    if (!flags[ZF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JNL:
    if (flags[SF] == flags[OF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JNLE:
    if (!flags[ZF] && flags[SF] == flags[OF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JNBE:
    if (!flags[CF] && !flags[ZF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case JMP:
    // JMP imm
    if (node->imm != NULL) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }

    // JMP r16
    if (*(node->nreg) == 1) {
      IP = regs[*(node->regs[0])];
      return EXIT_IPCHANGED;
    }

    break;
  case JL:
    if (flags[SF] != flags[OF]) {
      IP = *(node->imm);
      return EXIT_IPCHANGED;
    }
    break;
  case LEA:
    // LEA r16, mOp
    if (*(node->nreg) == 1 && node->mOp != NULL) {
      regs[*(node->regs[0])] = mOp;
      return EXIT_CONTINUE;
    }
    break;
  case MOV:
    if (*(node->nreg) == 2) {
      // MOV r16, r16
      regs[*(node->regs[0])] = regs[*(node->regs[1])];
      return EXIT_CONTINUE;
    }

    if (*(node->nreg) == 1 && node->mOp != NULL) {
      if (!strcmp(node->op1, node->mOp)) {
        if (*(node->regs[0]) < 8) {
          // MOV mOp, r16
          memcpy(data_mem + mOp, regs + *(node->regs[0]), 2);
          return EXIT_CONTINUE;
        } else if (*(node->regs[0]) > 7 && *(node->regs[0]) < 12) {
          // MOV mOp, r8
          memcpy(data_mem + mOp, regs + *(node->regs[0]) - 8, 1);
          return EXIT_CONTINUE;
        }
      } else {
        if (*(node->regs[0]) < 8) {
          // MOV r16, mOp
          memcpy(regs + *(node->regs[0]), data_mem + mOp, 2);
          return EXIT_CONTINUE;
        } else if (*(node->regs[0]) > 7 && *(node->regs[0]) < 12) {
          // MOV r8, mOp
          memcpy(regs + *(node->regs[0]) - 8, data_mem + mOp, 1);
          return EXIT_CONTINUE;
        }
      }
    }

    // MOV r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      regs[*(node->regs[0])] = *(node->imm);
      return EXIT_CONTINUE;
    }

    // MOV mOp, imm16
    if (*(node->nreg) == 0 && node->mOp != NULL && node->imm != NULL) {
      memcpy(data_mem + mOp, node->imm, 2);
      return EXIT_CONTINUE;
    }
    break;
  case NEG:
    if (*(node->nreg) == 1) {
      // NEG r16
      process_operation(&regs[*(node->regs[0])], NULL, OP_NEG);
      return EXIT_CONTINUE;
    }
    break;
  case OR:
    if (*(node->nreg) == 2) {
      // OR r16, r16
      process_operation(&regs[*(node->regs[0])], &regs[*(node->regs[1])],
                        OP_OR);
      return EXIT_CONTINUE;
    }

    // OR r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(&regs[*(node->regs[0])], node->imm, OP_OR);
      return EXIT_CONTINUE;
    }

    // OR mOp, imm16
    if (*(node->nreg) == 0 && node->mOp != NULL && node->imm != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), node->imm, OP_OR);
      return EXIT_CONTINUE;
    }

    // OR r16, mOp
    if (*(node->nreg) == 1 && node->mOp != NULL) {
      process_operation(&regs[*(node->regs[0])], (uint16_t *)(data_mem + mOp),
                        OP_OR);
      return EXIT_CONTINUE;
    }
    break;
  case POP:
    if (*(node->nreg) == 1) {
      // POP r16
      regs[*(node->regs[0])] = pop16_stack();
      return EXIT_CONTINUE;
    }
    break;
  case PUSH:
    if (*(node->nreg) == 1) {
      // PUSH r16
      push16_stack(regs[*(node->regs[0])]);
      return EXIT_CONTINUE;
    }

    if (node->mOp != NULL) {
      // PUSH mOp
      push16_stack(*(uint16_t *)(data_mem + mOp));
      return EXIT_CONTINUE;
    }
    break;
  case RET:
    // RET imm16
    if (node->imm != NULL) {
      IP = pop16_stack();
      *sp += *(node->imm);
      return EXIT_IPCHANGED;
    }

    // RET
    IP = pop16_stack();
    return EXIT_IPCHANGED;
  case SAR:
    // SAR r16, CL
    if (*(node->nreg) == 2) {
      // Get the number of times to shift from the lower 8 bits of CX
      uint16_t *shift = malloc(sizeof(uint16_t));
      *shift = regs[2] & 0xFF; // CX && 0xFF = CL
      // printf("*shift: %x\n", *shift);
      process_operation(&regs[*(node->regs[0])], shift, OP_SAR);
      return EXIT_CONTINUE;
    }

    break;
  case SHL:
    // SHL r16, 1
    if (*(node->nreg) == 1 && node->imm != NULL && *(node->imm) == 1) {
      uint16_t *shift = malloc(sizeof(uint16_t));
      *shift = 1;
      process_operation(&regs[*(node->regs[0])], shift, OP_SHL);
      return EXIT_CONTINUE;
    }

    // SHL r16, CL
    if (*(node->nreg) == 2) {
      // Get the number of times to shift from the lower 8 bits of CX
      uint16_t *shift = malloc(sizeof(uint16_t));
      *shift = regs[2] & 0xFF; // CX && 0xFF = CL
      process_operation(&regs[*(node->regs[0])], shift, OP_SHL);
      return EXIT_CONTINUE;
    }
    break;
  case SUB:
    // SUB r16, imm16
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(regs + *(node->regs[0]), node->imm, OP_MINUS);
      return EXIT_CONTINUE;
    }

    // SUB mOp, imm16
    if (*(node->nreg) == 0 && node->mOp != NULL && node->imm != NULL) {
      process_operation((uint16_t *)(data_mem + mOp), node->imm, OP_MINUS);
      return EXIT_CONTINUE;
    }

    // SUB r16, mOp
    if (*(node->nreg) == 1 && node->mOp != NULL) {
      // uint16_t *op1_ptr = (uint16_t *)(regs + *(node->regs[0]));
      process_operation(&regs[*(node->regs[0])], (uint16_t *)(data_mem + mOp),
                        OP_MINUS);
      return EXIT_CONTINUE;
    }
    break;
  case TEST:
    // TEST r16, r16
    if (*(node->nreg) == 2) {
      process_operation(&regs[*(node->regs[0])], &regs[*(node->regs[1])],
                        OP_TEST);
      return EXIT_CONTINUE;
    }

    if (*(node->nreg) == 1 && node->imm != NULL) {
      if (*node->regs[0] < 8) {
        // TEST r16, imm16
        process_operation(&regs[*(node->regs[0])], node->imm, OP_TEST);
        // printf("TEST - TO BE PATCHED\n");
        return EXIT_CONTINUE;
      } else if (*(node->regs[0]) > 7 && *(node->regs[0]) < 12) {
        // TEST r8, imm16
        process_operation((uint16_t *)(regs + *(node->regs[0]) - 8), node->imm,
                          OP_TEST);
        return EXIT_CONTINUE;
      }
    }

    // TEST mOp, imm16
    if (*(node->nreg) == 0 && node->mOp != NULL && node->imm != NULL) {
      uint16_t *op1_ptr = (uint16_t *)(data_mem + mOp);

      if (node->spe != NULL) {
        if (!strcmp(node->spe, "byte")) {
          uint8_t op1 = (uint8_t)(*op1_ptr);
          process_operation((uint16_t *)&op1, node->imm, OP_TEST);
          return EXIT_CONTINUE;
        }
      } else {
        process_operation(op1_ptr, node->imm, OP_TEST);
        return EXIT_CONTINUE;
      }
    }
    break;
  case XCHG:
    // XCHG r16, r16
    if (*(node->nreg) == 2) {
      uint16_t tmp = regs[*(node->regs[0])];
      regs[*(node->regs[0])] = regs[*(node->regs[1])];
      regs[*(node->regs[1])] = tmp;
      return EXIT_CONTINUE;
    }
    break;
  case XOR:
    if (*(node->nreg) == 2) {
      if (*(node->regs[0]) == *(node->regs[1])) {
        if (*(node->regs[0]) < 8) {
          // XOR r16, r16
          regs[*(node->regs[0])] = 0;
          set_flag(ZF, 1);
          set_flag(CF, 0); // ?? maybe
          set_flag(SF, 0); // ?? maybe
          return EXIT_CONTINUE;
        } else if (*(node->regs[0]) > 11) {
          // XOR r8, r8 (high)

          // access the high byte of the register & set it to 0
          uint8_t *highByte = (uint8_t *)&regs[*(node->regs[0]) - 12] + 1;
          *highByte = 0;

          set_flag(ZF, 1);
          set_flag(CF, 0); // ?? maybe
          set_flag(SF, 0); // ?? maybe
          return EXIT_CONTINUE;
        }
      }
    }
    break;
  default:
    errx(1, "Undefined instruction!");
  }

  return EXIT_CONTINUE;
}
#include "interpreter.h"

void set_flag(char f, char v) {
  static const char flagChars[] = {'O', 'S', 'Z', 'C'};
  flags[f] = v ? flagChars[f] : '-';
}

void update_flags(uint16_t result) { // might need to optimize the checks
  // Set the OF flag
  // TODO when encountered

  // Set the SF flag
  set_flag(SF, (int16_t)result < 0);

  // Set the ZF flag
  set_flag(ZF, result == 0);

  // Set the CF flag
  set_flag(CF, result > 0xffff);
}

void process_operation(uint16_t *op1, uint16_t *op2, char op) {
  switch (op) {
  case OP_CMP:
    set_flag(ZF, *op1 == 0);
    set_flag(CF, *op1 - *op2 < 0);
    break;

  case OP_AND:
    uint16_t and = *op1 & *op2;
    set_flag(SF, (int16_t) and < 0);
    set_flag(ZF, and == 0);
    set_flag(CF, and > 0xffff);
    break;

  case OP_PLUS:
    *op1 += *op2;
    set_flag(SF, (int16_t)*op1 < 0);
    set_flag(ZF, *op1 == 0);
    set_flag(CF, (uint32_t)(*op1) > 0xffff); // should be that, never seen
    break;

  case OP_MINUS:
    *op1 -= *op2;
    // set_flag(SF, (int16_t)*op1 < 0);
    // set_flag(ZF, *op1 == 0);
    // set_flag(CF, *op1 - *op2 < 0);
    break;

  case OP_MULT:
    printf("MULT\n");
    // *op1 *= *op2;
    break;

  case OP_DIV:
    printf("DIV\n");
    // *op1 /= *op2;
    break;
  }
}

const char *get_syscall_type(uint16_t type) {
  for (size_t i = 0; i < SYSCALLS_SIZE; ++i) {
    if (type == syscallTypes[i].type) {
      return syscallTypes[i].name;
    }
  }

  // Unknown syscall type
  errx(1, "Unknown syscall type: %d", type);
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
    int n = atoi(nstr);

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

  // If the operand doesn't contain a "+" or "-" sign, check if it's a register
  int registerIndex = get_index(registers, REG_SIZE, copiedOp);
  if (registerIndex != -1)
    return (uint16_t)regs[registerIndex];

  // Otherwise, directly return the value of the memory address
  return (uint16_t)strtol(copiedOp, NULL, 16);
}

char *get_memory_content(unsigned char *data, size_t adr) {
  // Allocate memory for the memory content display
  char *content = malloc(32 * sizeof(char));
  if (content == NULL)
    errx(1, "Can't allocate memory for the memory content!");

  // Format the memory content string
  strcpy(content, " ;[");
  sprintf(content + strlen(content), "%04lx]", adr);

  // Add the memory content to the string (for now only 2 bytes)
  sprintf(content + strlen(content), "%02x", data[adr + 1]);
  sprintf(content + strlen(content), "%02x", data[adr]);

  // Create a copy of the string using strdup
  char *copiedContent = strdup(content);
  if (copiedContent == NULL)
    errx(1, "Can't allocate memory for copied content!");

  // Free the original content
  free(content);

  // Set the final formatted string back to node->mOp
  return copiedContent;
}

void get_msg(Message *msg_struct, uint16_t *msg) {
  // Check if the message is a syscall
  if (msg[0] != 0x01)
    errx(1, "Message is not a syscall"); // dk otherwise

  // Fill the message structure
  msg_struct->t = msg[1];
  msg_struct->nbytes = msg[3];
  msg_struct->data = msg[5];
}

int interpret(NodeAST *node) {
  // Get the memory content of the eventual memory operand
  uint16_t mOp = 0;
  char *memory_content = NULL;
  if (node->mOp != NULL) {
    mOp = process_memory_operand(node);
    memory_content = strdup(get_memory_content(data_mem, mOp));
  }

  // Print the registers status
  print_regs_status(regs, flags, node->ASM, memory_content);

  // Interpret the instruction
  switch (get_index(instructions, INSTR_SIZE, node->opC)) {
  case ADC:
    // ADC macro logic here
    break;
  case ADD:
    // ADD r16, r16
    if (*(node->nreg) == 2) {
      process_operation(&regs[*(node->regs[0])], &regs[*(node->regs[1])],
                        OP_PLUS);
      break;
    }
    break;
  case AND:
    // AND macro logic here
    break;
  case CALL:
    push16_stack(node->adr);
    IP = *(node->imm); // not sure, was for testing originally
    return EXIT_IPCHANGED;
  case CBW:
    // CBW macro logic here
    break;
  case CLD:
    // CLD macro logic here
    break;
  case CMP:
    // Compute the difference between op1 and op2 and update the flags
    // accordingly
    if (*(node->nreg) == 1 && node->imm != NULL) {
      process_operation(&regs[*(node->regs[0])], node->imm, OP_CMP);
      break;
    }

    break;
  case CWD:
    // CWD macro logic here
    break;
  case DEC:
    // DEC macro logic here
    break;
  case DIV:
    // DIV macro logic here
    break;
  case HLT:
    // HLT macro logic here
    break;
  case IN:
    // IN macro logic here
    break;
  case INC:
    // INC macro logic here
    break;
  case INT:
    // Get the message structure pointer from the BX register
    uint16_t currData[DATA_LINE_SIZE];
    memcpy(currData, data_mem + regs[BX], DATA_LINE_SIZE);

    // Process the current message
    Message *msg = malloc(sizeof(Message));
    if (msg == NULL)
      errx(1, "Could not allocate memory for message structure.");
    get_msg(msg, currData);

    // Print the syscall
    const char *syscall = get_syscall_type(msg->t); // will have to be rechecked
    size_t ret = get_syscall_return(msg->t);
    switch (msg->t) {
    case 1: // EXIT
      printf("<%s(%zu)>\n", syscall, msg->data);
      return EXIT_SYSCALL;

    case 4: // WRITE
      char *str = malloc(msg->nbytes + 1);
      memcpy(str, data_mem + msg->data, msg->nbytes);
      printf("<%s(1, 0x%04lx, %zu)%s => %zu>\n", syscall, msg->data,
             msg->nbytes, str, ret);
      break;

    case 17: // BRK, figure out what to do here
      // format <brk(0x0000) => 0>, check source files
      printf("<%s>\n", syscall);
      break;

    case 54: // IOCTL, figure out what to do here
      // format <ioctl(1, 0x0000, 0x0000)>, check source files
      printf("<%s>\n", syscall);
      break;
    }
    break;
  case JAE:
    // JAE macro logic here
    break;
  case JB:
    // JB macro logic here
    break;
  case JBE:
    // JBE macro logic here
    break;
  case JE:
    // JE macro logic here
    break;
  case JLE:
    // JLE macro logic here
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
    // JNL macro logic here
    break;
  case JNLE:
    // JNLE macro logic here
    break;
  case JNBE:
    // JNBE macro logic here
    break;
  case JMP:
    IP = *(node->imm);
    return EXIT_IPCHANGED;
    break;
  case JL:
    // JL macro logic here
    break;
  case LEA:
    // LEA r16, mOp
    regs[*(node->regs[0])] = mOp;
    break;
  case LOOP:
    // LOOP macro logic here
    break;
  case MOV:
    if (*(node->nreg) == 2) {
      // MOV r16, r16
      regs[*(node->regs[0])] = regs[*(node->regs[1])];
      break;
    }

    if (*(node->nreg) == 1 && node->mOp != NULL) {
      if (!strcmp(node->op1, node->mOp)) {
        // MOV mOp, r16
        memcpy(data_mem + mOp, regs + *(node->regs[0]), 2);
        break;
      } else {
        // MOV r16, mOp
        memcpy(regs + *(node->regs[0]), data_mem + mOp, 2);
        break;
      }
    }

    // MOV r16, imm16
    regs[*(node->regs[0])] = *(node->imm);
    break;
  case MUL:
    // MUL macro logic here
    break;
  case NEG:
    // NEG macro logic here
    break;
  case OR:
    // OR macro logic here
    break;
  case POP:
    // POP macro logic here
    break;
  case PUSH:
    // Push the value of the operand onto the stack
    if (*(node->nreg) == 1) {
      // PUSH r16
      push16_stack(regs[*(node->regs[0])]);
      break;
    }
    break;
  case RCL:
    // RCL macro logic here
    break;
  case REP:
    // REP macro logic here
    break;
  case RET:
    // RET macro logic here
    break;
  case SAR:
    // SAR macro logic here
    break;
  case SBB:
    // SBB macro logic here
    break;
  case SHL:
    // SHL macro logic here
    break;
  case SHR:
    // SHR macro logic here
    break;
  case STD:
    // STD macro logic here
    break;
  case SUB:
    if (*(node->nreg) == 1) {
      // SUB r16, imm16
      process_operation(regs + *(node->regs[0]), node->imm, OP_MINUS);
    } else {
      // SUB mOp, imm16
      uint16_t *op1_ptr = (uint16_t *)(data_mem + mOp);
      process_operation(op1_ptr, node->imm, OP_MINUS);
    }
    break;
  case TEST:
    // Process AND operation between op1 and op2 and update the flags
    if (*(node->nreg) == 1 && node->imm != NULL) {
      // TEST r8, imm16
      if (*(node->regs[0]) > 7 && *(node->regs[0]) < 12) {
        // Operate on the lower 8 bits of the register
        uint16_t *op1_ptr = (uint16_t *)(regs + *(node->regs[0]) - 8);
        uint8_t op1 = (uint8_t)(*op1_ptr);
        process_operation((uint16_t *)&op1, node->imm, OP_AND);
        break;
      }
    }

    break;
  case XCHG:
    // XCHG macro logic here
    break;
  case XOR:
    // XOR r16, r16
    regs[*(node->regs[0])] ^= regs[*(node->regs[1])];
    update_flags(regs[*(node->regs[0])]); // TDOO: modify ?
    break;
  case UNDEFINED:
    // UNDEFINED macro logic here
    break;
  default:
    errx(1, "Undefined instruction!");
  }

  return EXIT_CONTINUE;
}
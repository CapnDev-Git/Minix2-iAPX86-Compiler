#include "interpreter.h"

const char *instructions[INSTR_SIZE] = {
    "adc", "add",  "and", "call", "cbw",  "cld", "cmp",        "cwd",
    "dec", "div",  "hlt", "in",   "inc",  "int", "jae",        "jb",
    "jbe", "je",   "jle", "jnb",  "jne",  "jnl", "jnle",       "jnbe",
    "jmp", "jl",   "lea", "loop", "mov",  "mul", "neg",        "or",
    "pop", "push", "rcl", "rep",  "ret",  "sar", "sbb",        "shl",
    "shr", "std",  "sub", "test", "xchg", "xor", "(undefined)"};

size_t regs[REG_SIZE] = {0, 0, 0, 0, 0xffd2, 0, 0, 0};
char flags[FLAG_SIZE] = {'-', '-', '-', '-'};

void set_flag(char f, char v) { flags[f] = v ? v : '-'; }

void update_flags(uint16_t result) {
  // Set the ZF flag
  set_flag(ZF, result == 0);

  // Set the SF flag
  set_flag(SF, result < 0);

  // Set the OF flag
  set_flag(OF, result > 0xffff);

  // Set the CF flag
  set_flag(CF, result > 0xffff);
}

char *get_syscall_type(uint16_t type) {
  switch (type) {
  case 1:
    return EXIT;
  case 4:
    return WRITE;
  case 17:
    return BRK;
  case 54:
    return IOCTL;
  default:
    errx(1, "Unknown syscall type: %d", type);
  }
}

size_t get_syscall_return(uint16_t type) {
  switch (type) {
  case 1: // exit
    return 0;
  case 4: // write
    return 6;
  case 17: // brk
    return 0;
  case 54: // ioctl
    return -1;
  default:
    errx(1, "Unknown syscall type: %d", type);
  }
}

uint16_t process_memory_operand(NodeAST *node) {
  // TO MODIFY FOR OTHER MEMORY OPERANDS COMBINATIONS

  char *mOp = strdup(node->mOp);
  mOp++;                       // remove the first bracket
  mOp[strlen(mOp) - 1] = '\0'; // remove the last bracket
  return (uint16_t)strtol(mOp, NULL, 16);
}

char *get_memory_content(NodeAST *node, unsigned char *data,
                         size_t mOp_offset) {
  // Allocate memory for the memory content display
  char *content = malloc(32 * sizeof(char)); // true size is 13?
  if (content == NULL)
    errx(1, "Can't allocate memory for the memory content!");

  // Format the memory content string
  strcpy(content, " ;");
  strcat(content, node->mOp);

  // Add the memory content to the string (for now only 2 bytes)
  sprintf(content + strlen(content), "%02x", data[mOp_offset + 1]);
  sprintf(content + strlen(content), "%02x", data[mOp_offset]);

  // Add the end of string character
  content[strlen(content)] = '\0';
  return content;
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

int interpret(NodeAST *node, unsigned char *text, unsigned char *data) {
  size_t mOp = 0;

  // Get the memory content (if any)
  char *memory_content = NULL;
  if (node->mOp != NULL) {
    mOp = process_memory_operand(node);
    memory_content = strdup(get_memory_content(node, data, mOp));
  }

  // Print the registers status
  print_regs_status(regs, flags, node->ASM, memory_content);

  // Interpret the instruction
  switch (get_index(instructions, INSTR_SIZE, node->opC)) {
  case ADC:
    // ADC macro logic here
    break;
  case ADD:
    // ADD macro logic here
    break;
  case AND:
    // AND macro logic here
    break;
  case CALL:
    // CALL macro logic here
    break;
  case CBW:
    // CBW macro logic here
    break;
  case CLD:
    // CLD macro logic here
    break;
  case CMP:
    // CMP macro logic here
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
    memcpy(currData, data + regs[BX], DATA_LINE_SIZE);

    // Process the current message
    Message *msg = malloc(sizeof(Message));
    if (msg == NULL)
      errx(1, "Could not allocate memory for message structure.");
    get_msg(msg, currData);

    // Print the syscall
    char *syscall = get_syscall_type(msg->t);
    size_t ret = get_syscall_return(msg->t);
    switch (msg->t) {
    case 1: // EXIT
      printf("<%s(%zu)>\n", syscall, msg->data);
      return EXIT_FAILURE;

    case 4: // WRITE
      char *str = malloc(msg->nbytes + 1);
      memcpy(str, data + msg->data, msg->nbytes);
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

    default:
      errx(1, "Unknown syscall type: %d\n", msg->t);
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
    // JNB macro logic here
    break;
  case JNE:
    // JNE macro logic here
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
    // JMP macro logic here
    break;
  case JL:
    // JL macro logic here
    break;
  case LEA:
    // LEA macro logic here
    break;
  case LOOP:
    // LOOP macro logic here
    break;
  case MOV:
    // TODO: finish rest of MOV cases

    // MOV r16, r16
    if (*(node->len) == 2) {
      regs[*(node->regs[0])] = regs[*(node->regs[1])];
      break;
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
    // PUSH macro logic here
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
    // SUB mOp, imm1
    data[mOp] -= *(node->imm);
    update_flags(data[mOp]);
    break;
  case TEST:
    // TEST macro logic here
    break;
  case XCHG:
    // XCHG macro logic here
    break;
  case XOR:
    // XOR macro logic here
    break;
  case UNDEFINED:
    // UNDEFINED macro logic here
    break;
  default:
    errx(1, "Undefined instruction!");
  }

  return EXIT_SUCCESS;
}
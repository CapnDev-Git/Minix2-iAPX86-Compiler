#include "memory.h"

Vector get_args(int argc, char **argv) {
  Vector args;
  vector_init(&args);
  for (size_t i = 2; i < argc; ++i)
    vector_pushback(&args, argv[i]);
  return args;
}

Vector get_envp() {
  Vector envp;
  vector_init(&envp);
  vector_pushback(&envp, "PATH=/bin:/usr/bin");
  return envp;
}

void process_args(Vector *args, Vector *envp) {
  // Retrieve the first environment variable
  const unsigned char *env = envp->data[0];
  uint16_t env_head;

  // Define a vector to store the addresses of the arguments
  Vector addr;
  vector_init(&addr);

  // Calculate the length of the environment variable string + 1 for \0
  uint16_t len = strlen(env) + 1;

  // Calculate the length of each argument string + 1 for \0
  for (size_t i = 0; i < args->size; ++i)
    len += strlen(args->data[i]) + 1;

  // Ensure the length is even, and if not, add a null terminator to the stack
  if (len % 2)
    data_mem[--(*sp)] = '\0';

  // Push the environment variable onto the stack in reverse order
  data_mem[--(*sp)] = '\0'; // Separator between char & env pointer
  for (int i = strlen(env) - 1; i >= 0; --i) {
    data_mem[--(*sp)] = env[i]; // Push in reverse order to the
  }
  env_head = *sp; // Store the address of the environment variable

  // Push every argument onto the stack in reverse order (separated by '\0')
  for (int i = args->size - 1; i >= 0; --i) {
    const unsigned char *arg = args->data[i];
    data_mem[--(*sp)] = '\0'; // Add a null terminator between arguments
    for (int j = strlen(arg) - 1; j >= 0; --j)
      data_mem[--(*sp)] = arg[j]; // Push in reverse order to the stack
    vector_pushback(&addr, (void *)(uintptr_t)(*sp));
  }

  // Push delimiters between the character and environment pointer
  data_mem[--(*sp)] = '\0';
  data_mem[--(*sp)] = '\0';

  // Push the address of the environment variables onto the stack
  push16_stack(env_head);

  // Push delimiters between the character and argument pointers
  data_mem[--(*sp)] = '\0';
  data_mem[--(*sp)] = '\0';

  // Push the addresses of the arguments onto the stack
  for (size_t i = 0; i < args->size; ++i)
    push16_stack((uint16_t)((uintptr_t)addr.data[i]));

  // Push the number of arguments onto the stack
  push16_stack(args->size);
}

void setup_memory(int argc, char **argv) {
  // Get the arguments and environment variables
  Vector args = get_args(argc, argv);
  Vector envp = get_envp();

  // Process the arguments and environment variables
  process_args(&args, &envp);

  // Free the vectors
  vector_free(&args);
  vector_free(&envp);
}
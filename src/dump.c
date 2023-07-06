#include "dump.h"

void hexdump(const char *path) {
  // Initialize the buffer
  unsigned char *buffer = malloc(MEMORY_SIZE * sizeof(unsigned char));
  if (buffer == NULL)
    errx(1, "Can't allocate memory for the buffer!");

  // Open the file in binary mode
  FILE *pfile;
  pfile = fopen(path, "rb");
  if (pfile == NULL)
    errx(1, "Provided file can't be opened!");

  // Read the file into the buffer
  size_t read_bytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, pfile);
  if (read_bytes == 0)
    errx(1, "Provided file is empty!");

  // Close the file
  fclose(pfile);

  // Update by reference the text & data area sizes
  memcpy(&TEXT_SIZE, buffer + HEADER_PROGRAM_SIZE, 4);
  memcpy(&DATA_SIZE, buffer + HEADER_DATA_SIZE, 4);

  // Update by reference the buffer size
  TEXT_BEG = HEADER_SIZE;
  DATA_BEG = HEADER_SIZE + TEXT_SIZE;

  // Fill the memories with the buffer content
  memcpy(text_mem, buffer + HEADER_SIZE, TEXT_SIZE);
  memcpy(data_mem, buffer + DATA_BEG, DATA_SIZE);
}

char **get_cout(char *command, size_t *lc) {
  char **lines = NULL; // Array to store lines of command output
  char buffer[BUFFER_SIZE];

  FILE *pipe = popen(command, "r"); // Open a pipe to the command
  if (pipe == NULL)
    errx(1, "Command can't be executed!");

  // Read the command output line by line
  while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
    // Remove newline character from the end of the line
    buffer[strcspn(buffer, "\n")] = '\0';

    // Allocate memory for the new line
    char *line = malloc(strlen(buffer) + 1);
    strcpy(line, buffer);

    // Increase the size of the lines array
    lines = realloc(lines, (*lc + 1) * sizeof(char *));
    lines[*lc] = line;
    (*lc)++;
  }

  // Close the pipe
  pclose(pipe);

  // Add a null terminator to the last line
  if (*lc > 0) {
    lines = realloc(lines, (*lc + 1) * sizeof(char *));
    lines[*lc] = NULL;
  }

  return lines;
}
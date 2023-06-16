#include "dump.h"

void hexdump(const char *path, unsigned char *buffer, size_t *buffer_size,
             size_t *text_size, size_t *data_size) {
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

  // Update by reference the buffer size and the text area size
  memcpy(text_size, buffer + HEADER_PROGRAM_SIZE, 4);
  memcpy(data_size, buffer + HEADER_DATA_SIZE, 4);
  *buffer_size = read_bytes;
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
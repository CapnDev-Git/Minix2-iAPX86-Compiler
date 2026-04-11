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

  if (read_bytes < HEADER_SIZE)
    errx(1, "Provided file is too small to contain a valid header!");

  // Close the file
  fclose(pfile);

  // Update by reference the text & data area sizes
  memcpy(&TEXT_SIZE, buffer + HEADER_PROGRAM_SIZE, 4);
  memcpy(&DATA_SIZE, buffer + HEADER_DATA_SIZE, 4);

  if (TEXT_SIZE > MEMORY_SIZE || DATA_SIZE > MEMORY_SIZE ||
      HEADER_SIZE + TEXT_SIZE + DATA_SIZE > read_bytes)
    errx(1, "Provided file has invalid text/data sizes!");

  // Update by reference the buffer size
  TEXT_BEG = HEADER_SIZE;
  DATA_BEG = HEADER_SIZE + TEXT_SIZE;

  // Fill the memories with the buffer content
  memcpy(text_mem, buffer + HEADER_SIZE, TEXT_SIZE);
  memcpy(data_mem, buffer + DATA_BEG, DATA_SIZE);

  free(buffer);
}
#include "utest.h"

char *get_cout(const char *command) {
  char *buffer = (char *)malloc(MAX_LENGTH * sizeof(char));
  if (buffer == NULL) {
    printf("Memory allocation failed. Exiting.\n");
    return NULL;
  }

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    printf("Failed to execute command. Exiting.\n");
    free(buffer);
    return NULL;
  }

  char line[MAX_LENGTH];
  buffer[0] = '\0';
  char *newline;

  while (fgets(line, sizeof(line), fp) != NULL) {
    newline = strchr(line, '\n');
    if (newline != NULL) {
      *newline = '\0';
    }

    strncat(buffer, line, MAX_LENGTH - strlen(buffer) - 1);
    strcat(buffer, "\n");
  }

  pclose(fp);
  return buffer;
}

void cmp_files(const char *filename, int verbose, int interpret) {
  char mmvmCommand[MAX_LENGTH];
  snprintf(mmvmCommand, sizeof(mmvmCommand), "mmvm -%s %s %s",
           interpret ? "m" : "d", filename, interpret ? "2>&1" : "2>&1");

  char *ref = get_cout(mmvmCommand);
  if (ref == NULL)
    errx(1, "Failed to execute command: %s\n", mmvmCommand);

  // Construct the command to capture the output of the disassembler
  char disassemblerCommand[MAX_LENGTH];
  snprintf(disassemblerCommand, sizeof(disassemblerCommand),
           "./z123029_mmvm -%s %s %s", interpret ? "m" : "d", filename,
           interpret ? "" : "2>&1");

  // Capture the output of the disassembler
  char *disassemblerOutput = get_cout(disassemblerCommand);
  if (disassemblerOutput == NULL) {
    free(ref);
    errx(1, "Failed to execute command: %s\n", disassemblerCommand);
  }

  int match = strcmp(ref, disassemblerOutput);
  if (match == 0) {
    printOK(filename);
  } else {
    const char *errorLine = disassemblerOutput;
    while (*errorLine != '\0' && *errorLine == *ref) {
      ++errorLine;
      ++ref;
    }

    if (verbose) {
      printf("\nMissmatch for file %s:", filename);
      printDiff(errorLine, ref);
    } else {
      printKO(filename);
    }
  }
}

void cmp_files_dir(const char *directory, int verbose, int interpret) {
  DIR *dir = opendir(directory);
  if (dir == NULL) {
    printf("Failed to open directory: %s\n", directory);
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) { // Check if it's a regular file
      char filepath[MAX_LENGTH];
      snprintf(filepath, sizeof(filepath), "%s%s", directory, entry->d_name);
      cmp_files(filepath, verbose, interpret);
    }
  }

  closedir(dir);
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4)
    errx(1, "Usage: %s [-v|--verbose] <file|directory>\n", argv[0]);

  if (argv[1][0] == '-' && strcmp(argv[1], "--verbose") &&
      strcmp(argv[1], "-v") && strcmp(argv[1], "-d") && strcmp(argv[1], "-m"))
    errx(1, "Invalid flag: %s\n", argv[1]);

  if (argc == 3 && strcmp(argv[1], "--verbose") && strcmp(argv[1], "-v") &&
      strcmp(argv[1], "-d") && strcmp(argv[1], "-m"))
    errx(1, "Invalid flag: %s\n", argv[1]);

  // Check if the input is a directory
  int interpret = strcmp(argv[2], "-d");
  DIR *dir = opendir(argv[argc - 1]);
  if (dir != NULL) {
    closedir(dir);
    cmp_files_dir(argv[argc - 1], argc == 3, interpret);
  } else {
    // Assume the input is a single file
    cmp_files(argv[argc - 1], argc == 3, interpret);
  }

  return 0;
}

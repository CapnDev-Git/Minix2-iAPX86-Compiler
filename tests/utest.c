#define _POSIX_C_SOURCE 200809L

#include "utest.h"

#include <sys/stat.h>

static int is_verbose_flag(const char *arg) {
  return !strcmp(arg, "-v") || !strcmp(arg, "--verbose");
}

static int is_mode_flag(const char *arg) {
  return !strcmp(arg, "-d") || !strcmp(arg, "-m");
}

static const char *get_reference_binary(void) {
  const char *reference = getenv("MMVM_REF");
  if (reference != NULL && reference[0] != '\0')
    return reference;
  return "mmvm";
}

static int output_indicates_missing_command(const char *output,
                                            const char *command) {
  return output != NULL && strstr(output, "not found") != NULL &&
         strstr(output, command) != NULL;
}

static char *capture_command_output(const char *command) {
  char *output = get_cout(command);
  if (output == NULL)
    errx(1, "Failed to execute command: %s\n", command);

  return output;
}

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
  size_t used = 0;

  while (fgets(line, sizeof(line), fp) != NULL) {
    newline = strchr(line, '\n');
    if (newline != NULL) {
      *newline = '\0';
    }

    int written = snprintf(buffer + used, MAX_LENGTH - used, "%s\n", line);
    if (written < 0 || (size_t)written >= MAX_LENGTH - used)
      break;
    used += (size_t)written;
  }

  pclose(fp);
  return buffer;
}

void cmp_files(const char *filename, int verbose, int interpret) {
  char mmvmCommand[MAX_LENGTH];
  const char *referenceBinary = get_reference_binary();
  snprintf(mmvmCommand, sizeof(mmvmCommand), "%s -%s %s 2>&1", referenceBinary,
           interpret ? "m" : "d", filename);

  char *ref = capture_command_output(mmvmCommand);

  if (!getenv("MMVM_REF") && output_indicates_missing_command(ref, "mmvm")) {
    free(ref);
    snprintf(mmvmCommand, sizeof(mmvmCommand), "./z123029_mmvm -%s %s 2>&1",
             interpret ? "m" : "d", filename);
    ref = capture_command_output(mmvmCommand);
  }

  char *refBase = ref;

  // Construct the command to capture the output of the disassembler
  char disassemblerCommand[MAX_LENGTH];
  snprintf(disassemblerCommand, sizeof(disassemblerCommand),
           "./z123029_mmvm -%s %s 2>&1", interpret ? "m" : "d", filename);

  // Capture the output of the disassembler
  char *disassemblerOutput = capture_command_output(disassemblerCommand);
  char *disassemblerBase = disassemblerOutput;

  int match = strcmp(ref, disassemblerOutput);
  if (match == 0) {
    printOK(filename);
  } else {
    const char *errorLine = disassemblerBase;
    const char *refLine = refBase;
    while (*errorLine != '\0' && *errorLine == *refLine) {
      ++errorLine;
      ++refLine;
    }

    if (verbose) {
      printf("\nMissmatch for file %s:", filename);
      printDiff(errorLine, refLine, disassemblerBase, refBase);
    } else {
      printKO(filename);
    }
  }

  free(refBase);
  free(disassemblerBase);
}

void cmp_files_dir(const char *directory, int verbose, int interpret) {
  DIR *dir = opendir(directory);
  if (dir == NULL) {
    printf("Failed to open directory: %s\n", directory);
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    char filepath[MAX_LENGTH];
    snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

    struct stat entryStat;
    if (stat(filepath, &entryStat) == 0 && S_ISREG(entryStat.st_mode)) {
      cmp_files(filepath, verbose, interpret);
    }
  }

  closedir(dir);
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4)
    errx(1, "Usage: %s [-v|--verbose] [-d|-m] <file|directory>\n", argv[0]);

  int argi = 1;
  int verbose = 0;
  int interpret = 1;

  if (argi < argc && is_verbose_flag(argv[argi])) {
    verbose = 1;
    ++argi;
  }

  if (argi < argc && is_mode_flag(argv[argi])) {
    interpret = strcmp(argv[argi], "-d") != 0;
    ++argi;
  }

  if (argi != argc - 1)
    errx(1, "Usage: %s [-v|--verbose] [-d|-m] <file|directory>\n", argv[0]);

  // Check if the input is a directory
  DIR *dir = opendir(argv[argi]);
  if (dir != NULL) {
    closedir(dir);
    cmp_files_dir(argv[argi], verbose, interpret);
  } else {
    // Assume the input is a single file
    cmp_files(argv[argi], verbose, interpret);
  }

  return 0;
}

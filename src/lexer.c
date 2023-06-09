#include "lexer.h"

enum Token {
  INSTRUCTION,
  INDICATOR,
  REGISTER,
  NUMBER,
  COMMA,
  PLUS,
  MINUS,
  OPEN,
  CLOSE,
} typedef Token;

char **split_string(char *str, size_t *count) {
  char **groups = NULL;
  *count = 0;

  char *token = strtok(str, " ");
  while (token != NULL) {
    (*count)++;
    groups = realloc(groups, *count * sizeof(char *));
    if (groups == NULL) {
      *count = 0;
      return NULL;
    }
    groups[*count - 1] = strdup(token);
    token = strtok(NULL, " ");
  }

  return groups;
}

void free_groups(char **groups, size_t count) {
  for (size_t i = 0; i < count; i++)
    free(groups[i]);
  free(groups);
}

void lex(char **ASM, size_t lc) {
  char *line;
  char **groups;
  size_t count;

  for (size_t i = 0; i < lc; i++) {
    line = ASM[i];
    groups = split_string(line, &count);
    if (groups == NULL)
      errx(1, "Can't split the string!");

    /****************/
    /* PROCESS HERE */
    /****************/

    free_groups(groups, count);
  }
}
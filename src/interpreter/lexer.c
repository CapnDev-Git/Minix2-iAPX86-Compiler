#include "lexer.h"

char **lex(char *line, size_t *n) {
  char **tokens = NULL;
  *n = 0;

  char *token = strtok(line, " ");
  while (token != NULL) {
    size_t len = strlen(token);
    (*n)++;
    tokens = realloc(tokens, *n * sizeof(char *));
    if (tokens == NULL) {
      *n = 0;
      return NULL;
    }
    tokens[*n - 1] = strdup(token);
    token = strtok(NULL, " ");
  }
  return tokens;
}

void remove_separator(char *token) {
  size_t len = strlen(token);
  if (token[len - 1] == ',' || token[len - 1] == ':')
    token[len - 1] = '\0';
}
#include "lexer.h"

static char *dup_string(const char *source) {
  size_t len = strlen(source) + 1;
  char *copy = malloc(len);
  if (copy == NULL)
    errx(1, "Can't allocate memory for string duplication!");

  memcpy(copy, source, len);
  return copy;
}

char **lex(char *line, size_t *n) {
  char **tokens = NULL;
  *n = 0;

  char *token = strtok(line, " ");
  while (token != NULL) {
    (*n)++;
    char **new_tokens = realloc(tokens, *n * sizeof(char *));
    if (new_tokens == NULL) {
      free_2d(tokens, *n - 1);
      *n = 0;
      return NULL;
    }
    tokens = new_tokens;
    tokens[*n - 1] = dup_string(token);
    if (tokens[*n - 1] == NULL) {
      free_2d(tokens, *n);
      *n = 0;
      return NULL;
    }
    token = strtok(NULL, " ");
  }
  return tokens;
}

void remove_separator(char *token) {
  size_t len = strlen(token);
  if (len > 0 && (token[len - 1] == ',' || token[len - 1] == ':'))
    token[len - 1] = '\0';
}
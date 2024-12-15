#ifndef _LEXER_H_
#define _LEXER_H_

#include "tokens.h"
void print_token(struct Token token);
void free_token_array(struct Token* arr, int len); 
struct Token* lexer(char buf[1000], int* ln);
#endif

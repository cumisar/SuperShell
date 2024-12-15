#ifndef _PARSE_H_
#define _PARSE_H_


#include "nodes.h"
#include "lexer.h"
struct ShellNode* try_parse_shell_command(struct Token* tokens, int ln, int* out);

#endif

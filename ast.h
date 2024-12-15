#ifndef _AST_H_
#define _AST_H_
#include "nodes.h"
void print_shell_node(struct ShellNode* node, int indent);

void shell_free(struct ShellNode*);
void conditional_free(struct ConditionalNode*);
void pipeline_free(struct PipelineNode*);
void command_free(struct CommandNode*);

#endif

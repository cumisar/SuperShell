#ifndef _NODES_H_
#define _NODES_H_

#include <stdbool.h>

struct CommandNode{
    char* name;
    char** argv;
    char* file_in, *file_out;
    bool append;
};

typedef enum{
    COMMAND, 
    SHELL,
}NodeType;

struct PipelineNode{
    struct PipelineNode* pipeline;
    NodeType type;
    union{
        struct ShellNode* shell;
        struct CommandNode* command;
    };
};

struct ConditionalNode{
	bool if_false;
    struct PipelineNode* pipeline;
    struct ConditionalNode* conditional;
};

struct ShellNode{
    bool background;
    struct ConditionalNode* conditional;
    struct ShellNode* shell;
};
#endif

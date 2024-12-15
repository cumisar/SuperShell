#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include "tokens.h"
#include "nodes.h"
#include <stdbool.h>


void print_indent(int indent);
void print_command_node(struct CommandNode* node, int indent);
void print_shell_node(struct ShellNode* node, int indent);
void print_conitional_node(struct ConditionalNode* node, int indent);
void print_pipeline_node(struct PipelineNode* node, int indent);

void shell_free(struct ShellNode*);
void conditional_free(struct ConditionalNode*);
void pipeline_free(struct PipelineNode*);
void command_free(struct CommandNode*);


void print_indent(int indent){
    for (int i = 0; i < indent; i++)
        putchar(' ');
}

void print_shell_node(struct ShellNode* node, int indent){
    if (!node){
        printf("nil");
        return;
    }
    printf("ShellNode{\n");
    print_indent(indent + 4);
    printf("background=%d,\n", node->background);
    print_indent(indent + 4);
    printf("conditional=");
    print_conitional_node(node->conditional, indent + 4);
    printf(",\n");
    print_indent(indent + 4);
    printf("shell=");
    print_shell_node(node->shell, indent + 4);
    printf(",\n");
    print_indent(indent);
    printf("}");
}

void print_conitional_node(struct ConditionalNode* node, int indent){
    if (!node){
        printf("nil");
        return;
    }
    printf("ConditionalNode{\n");
    print_indent(indent + 4);
    printf("pipeline=");
    print_pipeline_node(node->pipeline, indent + 4);
    printf(",\n");
    print_indent(indent + 4);
    
    if (node->if_false) printf("if_false: True,\n");
    else printf("if_false: False,\n");
    
    print_indent(indent+4);
    
    printf("conditional=");
    print_conitional_node(node->conditional, indent + 4);
    printf(",\n");
    print_indent(indent);
    printf("}");
}

void print_pipeline_node(struct PipelineNode* node, int indent){
    if (!node){
        printf("nil");
        return;
    }
    printf("PipelineNode{\n");
    print_indent(indent + 4);
    if (node->type == COMMAND) {
        printf("command=");
        print_command_node(node->command, indent + 4);
    } else {
        printf("shell=");
        print_shell_node(node->shell, indent + 4);
    }
    printf(",\n");
    print_indent(indent + 4);
    printf("pipeline=");
    print_pipeline_node(node->pipeline, indent + 4);
    printf(",\n");
    print_indent(indent);
    printf("}");
}

void print_command_node(struct CommandNode* node, int indent){
    if (!node){
        printf("nil");
        return;
    }
    printf("CommandNode{\n");
    print_indent(indent + 4);
    printf("name=`%s`,\n", node->name);
    print_indent(indent + 4);
    printf("args=[");
    for (int i = 0; node->argv[i]; i++)
        printf("`%s`,", node->argv[i]);
    printf("],\n");
    print_indent(indent + 4);
    printf("input=");
    if (node->file_in) {
        printf("`%s`", node->file_in);
    } else {
        printf("nil");
    }
    printf(",\n");
    print_indent(indent + 4);
    printf("output=");
    if (node->file_out) {
        printf("`%s`", node->file_out);
    } else {
        printf("nil");
    }
    printf(",\n");
    print_indent(indent + 4);
    printf("append=%d,\n", node->append);
    print_indent(indent);
    printf("}");
}


void shell_free(struct ShellNode* node){
		if (!node) return;
		
		if (node->conditional){
				conditional_free(node->conditional);
			}
		if (node->shell){
				shell_free(node->shell);
			}
		free(node);
	}
void conditional_free(struct ConditionalNode* node){
		if (!node){
				return;
			}
		if (node->pipeline){
				pipeline_free(node->pipeline);
			}
		if (node->conditional){
				conditional_free(node->conditional);
			}
		free(node);
	}
void pipeline_free(struct PipelineNode* node){
		if (!node){
				return;
			}
		if (node->pipeline){
				pipeline_free(node->pipeline);
			}
		if (node->type==SHELL){
				shell_free(node->shell);
			}
		if (node->type==COMMAND){
				command_free(node->command);
			}
	}
void command_free(struct CommandNode* node){
		for (int i=0; node->argv[i]; i++){
				free(node->argv[i]);
			}
		if (node->argv){
				free(node->argv);
			}
		if (node->file_in){
				free(node->file_in);
			}
		if (node->file_out){
				free(node->file_out);
			}
		free(node);
	}

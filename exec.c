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
#include "lexer.h"
#include "ast.h"
#include "parse.h"
int exec_shell(struct ShellNode *node);

int execute_buldin(struct CommandNode* node){
		if (!strcmp(node->name, "cd")){
				if (node-> argv[1]==NULL || node->argv[2]){
						printf("АШИПКА!!\n");
						return 1;
				}
				else {chdir(node->argv[1]); return 0;}
			}
	}


int exec_command(struct CommandNode* node){
		
	
		int input=0, output=1;
		if (node->file_in){
				input=open(node->file_in, O_RDONLY, 0400);
				if (input<0) {
						perror("open");
						//ошибка
						exit(1);
					}
				dup2(input, 0);
				close(input);
			}
		if (node->file_out){
				if (node->append){
						output=open(node->file_out, O_WRONLY| O_APPEND| O_CREAT, 0666);
						if (output<0) {
								perror("open");
								//ошибка
								exit(1);
							}						
					}
				else{
						output=open(node->file_out, O_WRONLY| O_TRUNC | O_CREAT, 0666);
						if (output<0) {
								perror("open");
								//ошибка
								exit(1);
							}	
					}
				dup2(output, 1);
				close(output);
			}
			
		execvp(node->name, node->argv);
		perror("execvp");
		exit(1);
	}
	

bool is_builtin(struct CommandNode* node){
	return !strcmp(node->argv[0], "cd");
}

int num_pipeline_nodes(struct PipelineNode * node){
		if (!node) return 0;
		return 1+num_pipeline_nodes(node->pipeline);
	}

int exec_pipeline(struct PipelineNode * node){
		int num=num_pipeline_nodes(node);
		
		if (num==1 && node->type==COMMAND && is_builtin(node->command)){
				return execute_buldin(node->command);
			} 
		
		
		int (*pipe_arr)[2];
		pipe_arr=malloc(sizeof(*pipe_arr)*(num-1));
		for (int i=0; i<num-1; i++){
				pipe(pipe_arr[i]);
			}
		pid_t pd;
		int i;
		for (i=0; i<num; i++, node=node->pipeline){
				if ((pd=fork())==0){
						signal(SIGINT, SIG_DFL);
						break;
					}
			}
		
		if (!pd){
				if (i){
						dup2(pipe_arr[i-1][0], 0);
					}
				if (i!=(num-1)){
						dup2(pipe_arr[i][1], 1);				
					}
				for (int k=0; k<(num-1); k++){
						close(pipe_arr[k][1]);
						close(pipe_arr[k][0]);
					}
				if (node->type==COMMAND){
						exit(exec_command(node->command));
					}
				else{
						exit(exec_shell(node->shell));
					}			
			
			}
		else {
				for (int k=0; k<(num-1); k++){
						close(pipe_arr[k][1]);
						close(pipe_arr[k][0]);
					}	
				int st;
				waitpid(pd, &st, 0);
				if (WIFEXITED(st) && WEXITSTATUS(st)){
						//exec command завершилась по exit(1)
						return st;
					}
				else return 0;		
			}
	}


int exec_conditional(struct ConditionalNode* node){
		int result;
		result=exec_pipeline(node->pipeline);
		if (node->conditional){
				if ((node->if_false && result!=0) || (!(node->if_false) && result==0)){
						result=exec_conditional(node->conditional);
						return result;
					}
				else{
					// не нужно выполнять conditional
						return result;
					}			
			}
	}
		
	
int exec_shell(struct ShellNode *node){
		int result;
		if (node->background){
				pid_t pd;
				if  ((pd=fork())==0){
						if ((pd=fork())==0){
								setpgid(0,0);
								signal(SIGINT, SIG_DFL);
								int blackhole=open("/dev/null", O_RDONLY);
								dup2(blackhole, 0);
								close(blackhole);
								result=exec_conditional(node->conditional);
							}
						exit(0);
					}
				else {
						int st;
						waitpid(pd, &st, 0);
					}
			}
		else {
				result=exec_conditional(node->conditional);
				if (node->shell){
						result=exec_shell(node->shell);
					}
			}
		return result;
	}

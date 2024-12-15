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
#include "nodes.h"
#include "tokens.h"
#include "ast.h"
#include <stdbool.h>
struct ShellNode* try_parse_shell_command(struct Token* tokens, int ln, int* out);



bool token_is_redirection(TokenOperator operator){
		if (operator==TOKEN_OPERATOR_RIGHT_ONE || operator==TOKEN_OPERATOR_RIGHT_TWO || operator==TOKEN_OPERATOR_LEFT){
					return true;
				}
		else return false;
	}


struct CommandNode* try_parse_command(struct Token* tokens, int ln, int *out){
		struct CommandNode* node =NULL;
		
		node=malloc(sizeof(*node)); //
		node->file_in=NULL; node->file_out=NULL;
		node->argv=malloc(sizeof(char*)); node->argv[0]=NULL; //
		
		int i=0, total_args_length=0;
		while ((i<ln) && ( ((tokens+i)->type==TOKEN_SYMBOL) || ( (tokens+i)->type==TOKEN_OPERATOR && token_is_redirection((tokens+i)->operator)))){ //
				if ((tokens+i)->type==TOKEN_SYMBOL){
						total_args_length++;
						node->argv=realloc(node->argv, total_args_length* sizeof(char*));
						node->argv[total_args_length-1]=calloc(sizeof(char*), strlen((tokens+i)->symbol)+1); //
						strcpy(node->argv[total_args_length-1], (tokens+i)->symbol);
						i++;
					}
				else {
						if ((tokens+i)->operator==TOKEN_OPERATOR_RIGHT_ONE){ //>
								i++;
								if ((i>=ln) || ((tokens+i)->type!=TOKEN_SYMBOL)){// если не символ все чищу и ошибка
										for (int k=0; k<total_args_length; k++){
												free(node->argv[k]); //
											}
										free(node->argv);
										free(node); //
										*out=0;
										return NULL;
									}
								else{
										node->file_out=calloc(sizeof(char*), strlen((tokens+i)->symbol)+1);
										strcpy(node->file_out,(tokens+i)->symbol);
										node->append=0;
										i++;
									}
							}
						if ((tokens+i)->operator==TOKEN_OPERATOR_RIGHT_TWO){ // >>
								i++;
								if ((i>=ln) || ((tokens+i)->type!=TOKEN_SYMBOL)){// если не символ все чищу и ошибка
										for (int k=0; k<total_args_length; k++){
												free(node->argv[k]); //
											}
										free(node->argv);
										free(node); //
										*out=0;
										return NULL;
									}
								else{
										node->file_out=calloc(sizeof(char*), strlen((tokens+i)->symbol)+1);
										strcpy(node->file_out,(tokens+i)->symbol);
										node->append=1;
										i++;
									}
							}
						if ((tokens+i)->operator==TOKEN_OPERATOR_LEFT){ // <
								i++;
								if ((i>=ln) || ((tokens+i)->type!=TOKEN_SYMBOL)){// если не символ все чищу и ошибка
										for (int k=0; k<total_args_length; k++){
												free(node->argv[k]);
											}
										free(node->argv);
										free(node); //
										*out=0;
										return NULL;
									}
								else{
										node->file_in=calloc(sizeof(char*), strlen((tokens+i)->symbol)+1);
										strcpy(node->file_in,(tokens+i)->symbol);
										node->append=0;
										i++;
									}
							}							
					} 
			} //while кончился 
			if (i==0) { // если не получилось распарсить как команду значит надо как шелл
					free(node); //
					*out=0;
					return NULL;		
				}
			else {
					total_args_length++; //
					node->name=node->argv[0];
					node->argv=realloc(node->argv, total_args_length* sizeof(char*));// 
					node->argv[total_args_length-1]=NULL; //
					*out=i; //вот тут точно ли i?
					return node;
				}

	}

struct PipelineNode* try_parse_pipeline(struct Token* tokens, int ln, int* out){
		struct PipelineNode* node=NULL;
		struct ShellNode * shell=NULL;
		struct CommandNode* command=NULL;
		
		int total_count=0, count=0;
		
		command=try_parse_command(tokens, ln, &total_count);
		ln-=total_count; tokens+=total_count;
		if (!command){
				int at;
				if (tokens->type==TOKEN_SYNTAX && tokens->syntax==CLOSE_BRACKETS) {
						//ошибка
						printf("АШИПКА!!\n");
						return NULL;
					}
				else if (tokens->type==TOKEN_SYNTAX && tokens->syntax==OPEN_BRACKETS){
						int brackets=1;
						for (at=1; at<ln; at++){
							if ((tokens+at)->type==TOKEN_SYNTAX && (tokens+at)->syntax==OPEN_BRACKETS){
									brackets++;
								}
							else if ((tokens+at)->type==TOKEN_SYNTAX && (tokens+at)->syntax==CLOSE_BRACKETS){
									brackets--;
							}
							if (!brackets) break;
						}
						if (at==ln){ 
								printf("АШИПКА!!\n");
								//ошибка
								*out=0;
								return NULL;
							}
						//иначе все хорошо и выходим	
					}
				else {
						//ошибка не ( и не )
						printf("АШИПКА!!\n");
						*out=0;
						return NULL;
					}
				ln-=2; tokens+=1; total_count +=2;
				
				shell=try_parse_shell_command(tokens, at-1, &count);
				total_count+=count;
				
				if (!shell) {*out=0; return NULL;}
				ln-=count; tokens+=count+1;
			}
		if (ln==0){
				node=malloc(sizeof(*node));
				if (command){
						node->type=COMMAND;
						node->command=command;
					}
				else if (shell){
						node->type=SHELL;
						node->shell=shell;
					}
				node->pipeline=NULL; //
				
				*out=total_count;
				return node;
			}	
		else if ((shell || command) && tokens->type==TOKEN_OPERATOR && tokens->operator==TOKEN_OPERATOR_PIPE){
				total_count++; 
				tokens++; ln--;
				
				node=malloc(sizeof(*node));
				if (command){
						node->type=COMMAND;
						node->command=command;
					}
				else if (shell){
						node->type=SHELL;
						node->shell=shell;
					}
					
				node->pipeline=try_parse_pipeline(tokens, ln, &count);
				
				if (node->pipeline==NULL){
						if (node->type==COMMAND){
								//ошибка
								printf("АШИПКА!!\n");
								command_free(command);
							}
						if (node->type==SHELL){
								//ошибка
								printf("АШИПКА!!\n");
								shell_free(shell);
							}
						*out=0;
						return NULL;
					}
				total_count+=count;
				*out=total_count;
				return node;				
			}
		else{
				node=malloc(sizeof(*node));
				if (command){
						node->type=COMMAND;
						node->command=command;
					}
				else if (shell){
						node->type=SHELL;
						node->shell=shell;
					}
				node->pipeline=NULL; //
				
				*out=total_count;
				return node;
			}
	
	} 

struct ConditionalNode* try_parse_conditional(struct Token* tokens, int ln, int* out){
		struct ConditionalNode* conditional=NULL;
		struct PipelineNode* pipeline=NULL;
		int total_count=0, count=0;
		
		pipeline=try_parse_pipeline(tokens, ln, &total_count);
		if (!pipeline) return NULL;
		
		ln-=total_count; tokens+=total_count;
		
		if (ln==0){
				conditional=malloc(sizeof(*conditional));
				conditional->pipeline=pipeline; conditional->conditional=NULL;
				*out=total_count;
				return conditional;
			}
		else if (tokens->type==TOKEN_OPERATOR && tokens->operator==TOKEN_OPERATOR_IF){
				total_count+=1;
				ln--; tokens++;
				
				conditional=malloc(sizeof(*conditional));
				conditional->pipeline=pipeline; conditional->if_false=false; 
				conditional->conditional=try_parse_conditional(tokens, ln, &count);
				if (conditional->conditional==NULL){
						pipeline_free(pipeline);
						*out=0;
						return NULL;
					}
				total_count+=count;
				*out=total_count;
				
				return conditional;		
				
			}
		else if (tokens->type==TOKEN_OPERATOR && tokens->operator==TOKEN_OPERATOR_ELIF){
				total_count+=1;
				ln--; tokens++;		
						
				conditional=malloc(sizeof(*conditional));
				conditional->pipeline=pipeline; conditional->if_false=true;
				conditional->conditional=try_parse_conditional(tokens, ln, &count);
				
				if (conditional->conditional==NULL){
						pipeline_free(pipeline);
						*out=0;
						return NULL;
					}
					
				total_count+=count;
				*out=total_count;
				
				return conditional;			
			}
		else {
			conditional=malloc(sizeof(*conditional));
			conditional->pipeline=pipeline; conditional->conditional=NULL;
			*out=total_count;
			return conditional;
			}
	}

struct ShellNode* try_parse_shell_command(struct Token* tokens, int ln, int* out){
		struct ConditionalNode* conditional=NULL;
		struct ShellNode *node =NULL;
		int total_count=0, count=0;
		conditional= try_parse_conditional(tokens, ln, &total_count);
		if (!conditional) {*out=0; return NULL;}
		ln-=total_count; tokens+=total_count; 
		if (ln==0){
				node=malloc(sizeof(*node));
				node->conditional=conditional; node->background=false; node->shell=NULL;
				*out=total_count; 
				return node;
			}
		else if (tokens->type==TOKEN_OPERATOR && tokens->operator==TOKEN_OPERATOR_POSL){
				total_count++; 
				tokens++; ln--;
				node=malloc(sizeof(*node));
				node->conditional=conditional; node->background=false;
				
				if (ln==0) return node;
				
				node->shell=try_parse_shell_command(tokens, ln, &count);
				
				if (node->shell==NULL){
						conditional_free(conditional);
						*out=0;
						return NULL;					
					}
				
				total_count+=count; 
				*out=total_count; 
				return node;
			}
		else if (tokens->type==TOKEN_OPERATOR  && tokens->operator==TOKEN_OPERATOR_FON){
				total_count++;
				tokens++; ln--;
				node=malloc(sizeof(*node));
				node->conditional=conditional; node->background=true; 
				
				if (ln==0) return node;
				
				node->shell=try_parse_shell_command(tokens, ln, &count);
				
				if (node->shell==NULL){
						conditional_free(conditional);
						*out=0;
						return NULL;					
					}
				
				total_count+=count; 
				*out=total_count; 
				return node;
			}
			
		else {
			//ошибка
				printf("АШИПКА!!\n");
				conditional_free(conditional);
				*out=0;
				return NULL;
			}
	} 

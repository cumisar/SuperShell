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
#include <time.h>
#include "tokens.h"
#include "lexer.h"
#include "ast.h"
#include "parse.h"
#include "exec.h"

#include <math.h>

char s[]="shell~";
char sh[]="shell~";

void setColor(int r, int g, int b) {
    printf("\033[38;2;%d;%d;%dm", r, g, b);
}

void resetColor() {
    printf("\033[0m");
}



int main(){
	char buf[1000]={0};
	struct Token* command;
	int command_len;
	int count=0; //для try_parse_shell
	struct ShellNode* ast;
	
	signal(SIGINT, SIG_IGN);
	printf("------Ctrl+D to exit------\n");
	int r, g, b;
    for (int i = 0; i < strlen(s); i++) {
        r = (int)(128 + 127 * sin(0.1 * i + 0));
    	g = (int)(128 + 127 * sin(0.1 * i + 2));
        b = (int)(128 + 127 * sin(0.1 * i + 4));
		setColor(r, g, b);
		printf("%c", s[i]);
	}
	//setColor(r, g, b);
	//printf("shell~ ");
	resetColor();
	while (fgets(buf, 1000, stdin)){
			printf("%s\n", buf);
			command=lexer(buf, &command_len);
			/*
			printf("Parse into tokens:\n ");
			for (int i=0; i<command_len; i++){
					print_token(command[i]);
					printf(" ");
			}
			*/
			//printf("\n");
			
			ast=try_parse_shell_command(command, command_len, &count);
			if (!ast) {printf("Error\n"); goto next;}
			//print_shell_node(ast, 0);
			//printf("Finished printind Shell Node\n");
			
			
			exec_shell(ast);
			free_token_array(command, command_len);
			
			shell_free(ast);
			//printf("Sucessfuly frees tree\n");
			
next:
			for (int i = 0; i < strlen(sh); i++) {
				r = (int)(128 + 127 * sin(0.1 * (time(0)+i) + 2));
				g = (int)(128 + 127 * sin(0.1 * (time(0)+i) + 4));
				b = (int)(128 + 127 * sin(0.1 * (time(0)+i) + 6));
				setColor(r, g, b);
				printf("%c", sh[i]);
			}
			//printf("shell~ ");
			resetColor();
		}
	 return 0;
	}

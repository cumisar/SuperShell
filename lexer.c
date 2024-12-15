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
#include <stdbool.h>

void print_token(struct Token token) {
	if (token.type == TOKEN_SYMBOL) printf("`%s`", token.symbol);

	if (token.type == TOKEN_SYNTAX && token.syntax == CLOSE_BRACKETS) printf("CLOSE_BRACKETS");
	if (token.type == TOKEN_SYNTAX && token.syntax == OPEN_BRACKETS) printf("OPEN_BRACKETS");

	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_PIPE) printf("TOKEN_OPERATOR_PIPE");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_RIGHT_ONE) printf("TOKEN_OPERATOR_RIGHT_ONE");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_RIGHT_TWO) printf("TOKEN_OPERATOR_RIGHT_TWO");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_LEFT) printf("TOKEN_OPERATOR_LEFT");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_IF) printf("TOKEN_OPERATOR_IF");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_ELIF) printf("TOKEN_OPERATOR_ELIF");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_FON) printf("TOKEN_OPERATOR_FON");
	if (token.type == TOKEN_OPERATOR && token.operator == TOKEN_OPERATOR_POSL) printf("TOKEN_OPERATOR_POSL");
}

void free_token_array(struct Token* arr, int len){
		for (int i=0; i<len; i++){
				if (arr[i].type==TOKEN_SYMBOL){
						free(arr[i].symbol);
					}
			}
		free(arr);
	}

const char* reserve=" \t\n()<>&|;";
const char* whitespace=" \t\n";

bool is_valid(char ch){
	for (int i=0; i<strlen(reserve); i++){
		if (ch==reserve[i]) return false;
	}
	return true;
}

bool if_whitespace(char c){
	for (int i=0; i<strlen(whitespace); i++){
		if (c==whitespace[i]) return true;
	}
	return false;
}

int try_parse_syntax(char* buf, struct Token* token){
	if (*buf==0) return 0;
	token->type=TOKEN_SYNTAX;
	if (*buf=='(') {
		token ->syntax=OPEN_BRACKETS; return 1;
	}
	if (*buf==')') {
		token ->syntax=CLOSE_BRACKETS; return 1;
	}
	return 0;
}
int try_parse_operator(char* buf, struct Token* token){
	if (*buf==0) return 0;
	token->type=TOKEN_OPERATOR;
	if (*buf=='|' && *(buf+1)=='|'){
		token->operator=TOKEN_OPERATOR_ELIF;
		return 2;
	}
	if (*buf=='&' && *(buf+1)=='&'){
		token->operator=TOKEN_OPERATOR_IF;
		return 2;
	}
	if (*buf=='>' && *(buf+1)=='>'){
		token->operator=TOKEN_OPERATOR_RIGHT_TWO;
		return 2;
	}
	if (*buf=='&'){
		token->operator=TOKEN_OPERATOR_FON;
		return 1;
	}
	if (*buf=='|'){
		token->operator=TOKEN_OPERATOR_PIPE;
		return 1;
	}
	if (*buf=='>'){
		token->operator=TOKEN_OPERATOR_RIGHT_ONE;
		return 1;
	}

	if (*buf== '<'){
		token->operator=TOKEN_OPERATOR_LEFT;
		return 1;
	}
	if (*buf== ';'){
		token->operator=TOKEN_OPERATOR_POSL;
		return 1;
	}

	return 0;
}

int try_parse_symbol(char* buf, struct Token* token){
	if (*buf==0) return 0;
	token->type=TOKEN_SYMBOL;
	char *start=buf;
	while (*buf && is_valid(*buf)){
		buf ++;	
	}
	token->symbol=calloc(sizeof(char), buf-start+1);
	memcpy(token->symbol, start, buf-start);
	return buf-start;
}

struct Token* lexer(char buf[1000], int* ln){
 		int i=0, count=0, sdvg;
 		struct Token* arr=NULL;

 		while (buf[i]){
			while (if_whitespace(buf[i])) i++;
			arr=realloc(arr, sizeof(struct Token)*(count+1));
			if ((sdvg=try_parse_operator(buf+i, &arr[count]))>0){
				i+=sdvg;
			}
			else if ((sdvg=try_parse_syntax(buf+i, &arr[count]))>0){
				i+=sdvg;
			}
			else if ((sdvg=try_parse_symbol(buf+i, &arr[count]))>0){
				i+=sdvg;
			} else break;
			count++;
		}
		*ln=count;
		return arr;

}


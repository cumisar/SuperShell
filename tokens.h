#ifndef _TOKENS_H_
#define _TOKENS_H_

typedef enum{
	TOKEN_SYMBOL,
	TOKEN_OPERATOR,
	TOKEN_SYNTAX
} TokenType;

typedef enum{
	CLOSE_BRACKETS,
	OPEN_BRACKETS,
} TokenSyntax;

typedef enum{
	TOKEN_OPERATOR_PIPE,
	TOKEN_OPERATOR_RIGHT_ONE,
	TOKEN_OPERATOR_RIGHT_TWO,
	TOKEN_OPERATOR_LEFT, //||
	TOKEN_OPERATOR_IF, //&&
	TOKEN_OPERATOR_ELIF,
	TOKEN_OPERATOR_FON, //&
	TOKEN_OPERATOR_POSL,
	
}TokenOperator; 


struct Token{
	TokenType type;
	union{
		char* symbol;
		TokenOperator operator;
		TokenSyntax syntax;
	};
};
#endif

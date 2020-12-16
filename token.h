#ifndef TOKEN_H
#define TOKEN_H

#include <vector>
#include <string>

typedef enum
{
	TK_BAD = 0,
	TK_ADD,
	TK_SUB,
	TK_MUL,
	TK_DIV,
	TK_NUM,
	TK_LP,
	TK_RP
} TokenType;

typedef union
{
	int ival;
	float fval;
	double dval;
	char cval;
} Value;

struct Token
{
	TokenType tk;
	Value val;
	std::vector<char> literal;
	std::vector<Token*> children;
	
};

bool parse(const std::vector<char>& text, std::vector<Token*>& tokens);
bool parse_string(const std::string& text, std::vector<Token*>& tokens);
void print_token(const Token* ptk);
void print_tokens(const std::vector<Token*>& tks);

#endif


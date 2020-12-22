#include "token.h"
#include <stdlib.h>
#include <vector>
#include <iostream>


bool parse_lp(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == '(')
	{
		pToken = new Token;
		pToken->tk = TK_LP;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_rp(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == ')')
	{
		pToken = new Token;
		pToken->tk = TK_RP;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_add(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == '+')
	{
		pToken = new Token;
		pToken->tk = TK_ADD;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_sub(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == '-')
	{
		pToken = new Token;
		pToken->tk = TK_SUB;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_mul(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == '*')
	{
		pToken = new Token;
		pToken->tk = TK_MUL;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_div(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == '/')
	{
		pToken = new Token;
		pToken->tk = TK_DIV;
		pToken->val.cval = c;
		pToken->literal.push_back(c);
		++index;
		return true;
	}
	else
	{
		return false;
	}
}

bool parse_space(const std::vector<char> text, int& index, Token*& pToken)
{
	char c = text[index];
	if(c == ' ' || c == '\n')
	{
		++index;
		return true;
	}
	else
	{
		return false;
	}
	
}

bool parse_num(const std::vector<char> text, int& index, Token*& pToken)
{
	enum
	{
		INITIAL,
		INT_PART,
		FRAC_PART,
		DOT
	} State;	

	int old_idx = index;
	
	int cur = INITIAL;
	std::vector<char> li;
	while(true)
	{
		char c = text[index++];
		if(c >= '0' && c <= '9')
		{
			if(cur == INITIAL || cur == INT_PART)
			{
				cur = INT_PART;
				li.push_back(c);
			}
			else if(cur == DOT || cur == FRAC_PART)
			{
				cur = FRAC_PART;
				li.push_back(c);
			}
			else
			{
				index = old_idx;
				return false;
			}

		}
		else if(c == '.')
		{
			if(cur == INT_PART)
			{
				cur = DOT;
				li.push_back(c);
			}
			else
			{
				index = old_idx;
				return false;
			}
		}
		else if((c < '0' || c > '9') && c != '.')
		{
			if(cur == INT_PART || cur == FRAC_PART)
			{
				--index;
				pToken = new Token;
				pToken->tk = TK_NUM;
				pToken->literal = li;
				sscanf(pToken->literal.data(), "%lf", &(pToken->val.dval));
				return true;
			}
			else
			{
				index = old_idx;
				return false;
			}

		}
		else
		{
			index = old_idx;
			return false;
		}
		
	}
}

bool parse(const std::vector<char>& text, std::vector<Token*>& tokens)
{
	int idx = 0;
	while(idx < text.size())
	{
		Token* pToken = 0;
		if(parse_lp(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_rp(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_add(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_sub(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_mul(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_div(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_num(text, idx, pToken))
		{
			tokens.push_back(pToken);
		}
		else if(parse_space(text, idx, pToken))
		{
		}
		else
		{
			std::cout << "Parse Error!" << std::endl;
			return false;
		}
	}
	return true;
}

bool parse_string(const std::string& text, std::vector<Token*>& tokens)
{
	std::vector<char> vtext;
	for(int i = 0; i < text.size(); ++i)
	{
		vtext.push_back(text[i]);
	}
	return parse(vtext, tokens);
}

void print_token(const Token* ptk)
{
	std::string li(ptk->literal.data(), ptk->literal.size());
	std::cout << "tk:" << ptk->tk << ", lit:" << li << std::endl;
}

void print_tokens(const std::vector<Token*>& tks)
{
	for(int i = 0; i < tks.size(); ++i)
	{
		Token* ptk = tks[i];
		print_token(ptk);
	}
}

void test()
{
	std::string t("12+45.6");
	std::vector<Token*> tokens;
	parse_string(t, tokens);
	print_tokens(tokens);

	std::cout << "----------------------------" << std::endl;
	tokens.clear();
	t = ("34/(45+100.123)");
	parse_string(t, tokens);
	print_tokens(tokens);

	std::cout << "----------------------------" << std::endl;
	tokens.clear();
	t = ("a834/45+100.123");
	parse_string(t, tokens);
	print_tokens(tokens);
}

/*
int main()
{
	test();
	return 0;
}
*/


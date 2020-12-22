#include "token.h"
#include <iostream>
#include <cassert>
#include <queue>
#include <cstring>
#include <sstream>
#include <map>
#include <cstdio>

// block : expression CR
// expression : term | expression + term | expression - term
// term : primary | term * primary | term / primary
// primary : num | (expression)

void print_tree(Token* node, const std::string& dotfile="tree.dot")
{
	std::stringstream ss;
	ss << "graph\n{\n\tlabel=\"" << dotfile << "\"\n";
	std::queue<Token*> que;
	que.push(node);
	int nodeidx = 0;
	std::string nodename(node->literal.data(), node->literal.size());
	std::map<Token*, int> tokenidxs;
	tokenidxs[node] = nodeidx;
	++nodeidx;
	while(que.size() > 0)
	{
		Token* n = que.front();
		que.pop();
		std::string li(n->literal.data(), n->literal.size());
		ss << "\tnode" << tokenidxs[n] << "[label=\"" << li << "\"];\n";

		//std::cout << "|" << li << "|" << n->tk << std::endl;
		for(int i = 0; i < (n->children).size(); ++i)
		{
			tokenidxs[n->children[i]] = nodeidx;
			++nodeidx;
			ss << "\tnode" << tokenidxs[n] << "--" << "node" << tokenidxs[n->children[i]] << ";\n";
			que.push((n->children)[i]);
		}
	}
	ss << "}";
	std::cout << ss.str() << std::endl;
	FILE* fp = 0;
	fp = fopen(dotfile.c_str(), "w");
	if(fp)
	{
		std::string str = ss.str();
		fwrite(str.c_str(), 1, str.size(), fp);
	}
	fclose(fp);
}

double eval_tree(Token* node)
{
	if(node->tk == TK_NUM)
	{
		return node->val.dval;
	}
	else
	{
		double left = eval_tree(node->children[0]);
		double right = eval_tree(node->children[1]);
		if(node->tk == TK_ADD)
		{
			return left + right;
		}
		else if(node->tk == TK_SUB)
		{
			return left - right;
		}
		else if(node->tk == TK_MUL)
		{
			return left * right;
		}
		else if(node->tk == TK_DIV)
		{
			return left / right;
		}
		else
		{
			std::cout << "eval error!" << std::endl;
			return 0;
		}
	}
}


Token* reduce_from_left(const std::vector<Token*>* pvTokens)
{
	if(pvTokens->size() == 0)
	{
		std::cout << "error null" << std::endl;
		return 0;
	}
	if(pvTokens->size() == 1)
	{
		return (*pvTokens)[0];
	}
	int left_idx = 0;
	Token* pLeftNode = (*pvTokens)[left_idx];
	if(pLeftNode->tk == TK_LP)
	{
		int left_num = 1;
		++left_idx;
		std::vector<Token*>* pinner = new std::vector<Token*>();
		while(true)
		{
			Token* pt = (*pvTokens)[left_idx];
			if(pt->tk == TK_LP)
			{
				++left_num;
			}
			else if(pt->tk == TK_RP)
			{
				--left_num;
			}
			++left_idx;
			if(left_num > 0)
			{
				pinner->push_back(pt);
			}
			else
			{
				break;
			}
		}
		Token* r = reduce_from_left(pinner);
		std::vector<Token*>* args = new std::vector<Token*>();
		args->push_back(r);
		while(left_idx < (*pvTokens).size())
		{
			args->push_back((*pvTokens)[left_idx++]);
		}
		return reduce_from_left(args);
		
	}
			
	int mid_idx = left_idx + 1;
	int right_idx = left_idx + 2;
	int test_idx = left_idx + 3;
	if(right_idx == pvTokens->size() - 1)
	{
		Token* pLeftNode = (*pvTokens)[left_idx];
		Token* pMidNode = (*pvTokens)[mid_idx];
		Token* pRightNode = (*pvTokens)[right_idx];
		
		Token* pt = new Token;
		pt->tk = pMidNode->tk;
		pt->literal = pMidNode->literal;
		pt->children.push_back(pLeftNode);
		pt->children.push_back(pRightNode);

		return pt;
	}
	else 
	{
		Token* pLeftNode = (*pvTokens)[left_idx];
		Token* pMidNode = (*pvTokens)[mid_idx];
		Token* pRightNode = (*pvTokens)[right_idx];
		Token* pTestNode = (*pvTokens)[test_idx];
		
		if(pRightNode->tk == TK_LP)
		{
			int left_num = 1;
			++right_idx;
			std::vector<Token*>* pinner = new std::vector<Token*>();
			while(true)
			{
				Token* pt = (*pvTokens)[right_idx];
				if(pt->tk == TK_LP)
				{
					++left_num;
				}
				else if(pt->tk == TK_RP)
				{
					--left_num;
				}
				++right_idx;
				if(left_num > 0)
				{
					pinner->push_back(pt);
				}
				else
				{
					break;
				}
			}
			Token* r = reduce_from_left(pinner);
			std::vector<Token*>* args = new std::vector<Token*>();
			args->push_back(pLeftNode);
			args->push_back(pMidNode);
			args->push_back(r);
			while(right_idx < (*pvTokens).size())
			{
				args->push_back((*pvTokens)[right_idx++]);
			}
			return reduce_from_left(args);

		}
		else if(pMidNode->tk == TK_MUL || pMidNode->tk == TK_DIV)
		{
			Token* pt = new Token;
			pt->tk = pMidNode->tk;
			pt->literal = pMidNode->literal;
			pt->children.push_back(pLeftNode);
			pt->children.push_back(pRightNode);

			std::vector<Token*>* args = new std::vector<Token*>();
			args->push_back(pt);
			
			while(test_idx < pvTokens->size())
			{
				args->push_back((*pvTokens)[test_idx]);
				test_idx++;
			}
			return reduce_from_left(args);
		}
		else if(pMidNode->tk == TK_ADD || pMidNode->tk == TK_SUB)
		{
			if(pTestNode->tk == TK_ADD || pTestNode->tk == TK_SUB)
			{
				Token* pt = new Token;
				pt->tk = pMidNode->tk;
				pt->literal = pMidNode->literal;
				pt->children.push_back(pLeftNode);
				pt->children.push_back(pRightNode);

				std::vector<Token*>* args = new std::vector<Token*>();
				args->push_back(pt);
				
				while(test_idx < pvTokens->size())
				{
					args->push_back((*pvTokens)[test_idx]);
					test_idx++;
				}
				return reduce_from_left(args);
			}
			else if(pTestNode->tk == TK_MUL || pTestNode->tk == TK_DIV)
			{
				std::vector<Token*>* args = new std::vector<Token*>();
				while(right_idx < pvTokens->size())
				{
					args->push_back( (*pvTokens)[right_idx] );
					right_idx++;
				}
				Token* rs = reduce_from_left(args);
				pRightNode = rs;

				Token* pt = new Token;
				pt->tk = pMidNode->tk;
				pt->literal = pMidNode->literal;
				pt->children.push_back(pLeftNode);
				pt->children.push_back(pRightNode);

				return pt;
			}
			else
			{
				std::cout << "error" << std::endl;
			}
		}
		else
		{
			std::cout << "outer error" << std::endl;
		}
	}
	return 0;
}

int main()
{
	std::string t("3/4\n");
	std::cout << t << std::endl;
	std::cout << 3/4.0 << std::endl;
        std::vector<Token*> tokens;
        parse_string(t, tokens);
        std::cout << "-----Token----------" << std::endl;
        print_tokens(tokens);
        std::cout << "----------------------------" << std::endl;
	Token* rt = reduce_from_left(&tokens);
	std::cout << ">>>>>>Tree:>>>>>>>>>>" << std::endl;
	print_tree(rt, "tree1.dot");
	std::cout << "<<<<<<Value:<<<<<<<<<" << std::endl;
	double v = eval_tree(rt);
	std::cout << "val:" << v << std::endl;

	std::cout << "------------------------" << std::endl;
	t = "3/4*9+56/45.7\n";
	std::cout << t << std::endl;
	std::cout << 3/4.0*9+56/45.7 << std::endl;
        tokens.clear();
        parse_string(t, tokens);
        std::cout << "-----Token----------" << std::endl;
        print_tokens(tokens);
        std::cout << "----------------------------" << std::endl;
	rt = reduce_from_left(&tokens);
	std::cout << ">>>>>>Tree:>>>>>>>>>>" << std::endl;
	print_tree(rt, "tree2.dot");
	std::cout << "<<<<<<Value:<<<<<<<<<" << std::endl;
	v = eval_tree(rt);
	std::cout << "val:" << v << std::endl;

	std::cout << "------------------------" << std::endl;
	t = "3/(4*9)+56/45.7\n";
	std::cout << t << std::endl;
	std::cout << 3/(4.0*9)+56/45.7 << std::endl;
        tokens.clear();
        parse_string(t, tokens);
        std::cout << "-----Token----------" << std::endl;
        print_tokens(tokens);
        std::cout << "----------------------------" << std::endl;
	rt = reduce_from_left(&tokens);
	std::cout << ">>>>>>Tree:>>>>>>>>>>" << std::endl;
	print_tree(rt, "tree3.dot");
	std::cout << "<<<<<<Value:<<<<<<<<<" << std::endl;
	v = eval_tree(rt);
	std::cout << "val:" << v << std::endl;

	std::cout << "------------------------" << std::endl;
	t = "3/((4*9)+56)/45.7\n";
	std::cout << t << std::endl;
	std::cout << 3/((4.0*9)+56)/45.7 << std::endl;
        tokens.clear();
        parse_string(t, tokens);
        std::cout << "-----Token----------" << std::endl;
        print_tokens(tokens);
        std::cout << "----------------------------" << std::endl;
	rt = reduce_from_left(&tokens);
	std::cout << ">>>>>>Tree:>>>>>>>>>>" << std::endl;
	print_tree(rt, "tree4.dot");
	std::cout << "<<<<<<Value:<<<<<<<<<" << std::endl;
	v = eval_tree(rt);
	std::cout << "val:" << v << std::endl;



	return 0;
}

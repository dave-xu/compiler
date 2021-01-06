#include<string>
#include<vector>
#include<stack>
#include<map>
#include<queue>
#include<iostream>
#include<sstream>

int s_id = 0;
struct State;
struct Link
{
	int id;
	char in;
	State* state;
	Link()
	{
		id = s_id++;
		in = '\0';
		state = 0;
	}
};

struct State
{
	int id;
	char name;
	std::vector<Link*> links;

	State()
	{
		id = s_id++;
		name = '@';
	}
};

typedef enum
{
	TK_NORM = 0,
	TK_STAR
} TokenType;

struct Token
{
	TokenType tk;
	char val;
};

std::string formName(char c)
{
	if(c == '@')
	{
		return "A";
	}
	if(c == '^')
	{
		return "S";
	}
	if(c == '$')
	{
		return "E";
	}
	if(c == '#')
	{
		return "N";
	}
	return std::string(1, c);
}

void print_states(State* state, const std::string& dotfile="tree.dot")
{
	std::stringstream ss;
	ss << "digraph finite_state_machine\n{\n\tlabel=\"" << dotfile << "\"\n";
	ss << "\trankdir=LR;\n";
	ss << "\tnode [shape = circle];\n";
	std::queue<State*> que;
	que.push(state);
	int stateidx = 0;
	std::string statename = formName(state->name);

	std::map<State*, int> stateidxs;
	stateidxs[state] = stateidx;
	++stateidx;
	while(que.size() > 0)
	{
		State* n = que.front();
		que.pop();
		for(int i = 0; i < (n->links).size(); ++i)
		{
			Link* l = n->links[i];
			if(stateidxs.find(l->state) == stateidxs.end())
			{
				stateidxs[l->state] = stateidx;
				++stateidx;
				que.push(l->state);
			}

			std::string ls = formName(n->name);
			std::string rs = formName(l->state->name);
			std::string ln = formName(l->in);
			if(ls == ".")
			{
				ls = "dot";
			}
			if(rs == ".")
			{
				rs = "dot";
			}
			if(ln == ".")
			{
				ln = "dot";
			}
			

			ss << "\t" << ls << "->" << rs << "[ label = \"" << ln << "\"];\n";
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



Token* getToken(const std::string& p, int& idx)
{
	if(idx >= p.size())
	{
		return 0;
	}
	if(idx + 1 == p.size())
	{
		if(p[idx] == '*')
		{
			std::cout << "Get Token Error!" << std::endl;
			return 0;
		}
		Token* ret = new Token;
		ret->tk = TK_NORM;
		ret->val = p[idx];
		idx += 1;
		return ret;
	}
	char c = p[idx];
	char c_test = p[idx+1];
	if(c_test == '*')
	{
		Token* ret = new Token;
		ret->tk = TK_STAR;
		ret->val = c;
		idx += 2;
		return ret;
	}
	else
	{
		Token* ret = new Token;
		ret->tk = TK_NORM;
		ret->val = c;
		idx += 1;
		return ret;
	}
}

State* parsePattern(const std::string& p)
{
	State* pStart = new State;
	pStart->name = '^';
	std::stack<State*> stack;
	stack.push(pStart);

	int idx = 0;
	while(1)
	{
		if(idx >= p.size())
		{
			break;
		}
		Token* token = getToken(p, idx);
		if(token == 0)
		{
			break;
		}
		if(token->tk == TK_NORM)
		{
			std::vector<State*> tmpvector;
			State* s = new State;
			s->name = token->val;
			tmpvector.push_back(s);
			while(stack.size() > 0)
			{
				State* ps = stack.top();
				stack.pop();

				for(int k = 0; k < tmpvector.size(); ++k)
				{
					State* nextstate = tmpvector[k];
					Link* l = new Link;
					l->in = nextstate->name;
					l->state = nextstate;
					ps->links.push_back(l);
				}
				tmpvector.push_back(ps);
			}
			stack.push(s);
		}
		else if(token->tk == TK_STAR)
		{
			State* star = new State;
			star->name = token->val;
			Link* loop = new Link;
			loop->in = token->val;
			loop->state = star;
			star->links.push_back(loop);
			stack.push(star);
		}

	}

	State* e = new State;
	e->name = '$';
	std::vector<State*> tmpvector;
	tmpvector.push_back(e);
	while(stack.size() > 0)
	{
		State* ps = stack.top();
		stack.pop();
		for(int k = 0; k < tmpvector.size(); ++k)
		{
			State* nextstate = tmpvector[k];
			Link* l = new Link;
			l->in = nextstate->name;
			l->state = nextstate;
			ps->links.push_back(l);
		}
		tmpvector.push_back(ps);
	}
	return pStart;
}

bool doMatchString(const std::string& str, const State* state)
{
	if(str.size() == 0)
	{
		for(int i = 0; i < state->links.size(); ++i)
		{
			const Link* l = state->links[i];
			if(l->in == '$')
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		char c = str[0];
		bool flag = false;
		for(int i = 0; i < state->links.size(); ++i)
		{
			const Link* l = state->links[i];
			if(l->in == c || l->in == '.')
			{
				std::string left = str.substr(1);
				flag = flag || doMatchString(left, l->state);
			}
		}
		return flag;
	}
}

bool match(const std::string& str, const std::string& p)
{
	State* s = parsePattern(p);
	return doMatchString(str, s);
}

int main()
{
	State* s = parsePattern("mis*is*ip*.");
	print_states(s, "s.dot");
	std::cout << match("mississippi", "mis*is*ip*.") << std::endl;
	std::cout << match("ab", ".*") << std::endl;
	std::cout << match("aa", "a*") << std::endl;
	return 0;
}


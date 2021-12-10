#ifndef DEFS_H
#define DEFS_H
#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <cassert>
using namespace std;

// 报错并输出错误内容
#define ERR_LOG(...) {\
	fprintf(stderr, "%s:line %d:\t", __FILE__, __LINE__);	\
	fprintf(stderr, __VA_ARGS__);							\
	fprintf(stderr, "\n");									\
	throw "error";											\
}

# define T_NONE        	"\033[0m"  
# define T_BLACK       	"\033[0;30m"  
# define T_DARK_GRAY   	"\033[1;30m"  
# define T_BLUE        	"\033[0;34m"  
# define T_LIGHT_BLUE  	"\033[1;34m"  
# define T_GREEN       	"\033[0;32m"  
# define T_LIGHT_GREEN 	"\033[1;32m"  
# define T_CYAN        	"\033[0;36m"  
# define T_LIGHT_CYAN  	"\033[1;36m"  
# define T_RED         	"\033[0;31m"  
# define T_LIGHT_RED   	"\033[1;31m"  
# define T_PURPLE      	"\033[0;35m"  
# define T_LIGHT_PURPLE	"\033[1;35m"  
# define T_BROWN       	"\033[0;33m"  
# define T_YELLOW      	"\033[1;33m"  
# define T_LIGHT_GRAY  	"\033[0;37m"  
# define T_WHITE       	"\033[1;37m"  


namespace compiler {

	const int ERROR_STATE =	-1;   	// 错误状态, 作为dfa邻接矩阵的占位值
	const int EMPTY_CHAR = 0; 		// 空字符, 作为nfa空边
	const int END_CHAR = '#';		// 待分析字符串的结束字符, 作为语法分析终结标志
	const int CHARSET_SIZE = 128;	// 终结符ascii码最大范围, 值小于它的符号视为终结符, 否则为非终结符

	inline int toNonterminal(int symbol);	// 普通字符(终结符)转符号(非终结符)
	inline int toTerminal(int symbol);		// 符号(非终结符)转普通字符(终结符)
	inline bool isTerminal(int symbol);		// 是否为终结符
	inline bool isNonterminal(int symbol);	// 是否为非终结符
	
	struct Edge {					// nfa邻接表的边
		int symbol;
		int from;
		int to;
	};

	// 由于分析表通常稀疏，用键值对而不用邻接矩阵表示
	typedef multimap<int, int>	NFAnode;	// NFA节点 (key: symbol, val: {next_states})
	typedef map<int, int>		DFAnode;	// DFA节点 (key: symbol, val: next_state)
	typedef map<int, NFAnode> 	NFAgraph;	// NFA图 (key: state, val: node)
	typedef map<int, DFAnode>	DFAgraph;	// DFA图 (key: state, val: node)
	typedef vector<Edge>		EdgeTable;	// 邻接表

	// token
	struct Token {
		int type;
		string val;
	};

	// 产生式 (P -> Ab)
	struct Production {
		int symbol;			// left, nonterminal symbol
		vector<int> right;	// mixed with nonterminals and terminals
		Production(int symbol, vector<int> right): 
			symbol(symbol), right(right) {};

		bool operator<(const Production &p) const;
		bool operator==(const Production &p) const;
	};

	// 产生式项目 (P -> A·b)
	struct ProductionItem : Production {
		int dot;			// position of dot (0 ~ right.size())
		ProductionItem(int symbol, vector<int>right, int dot): 
			Production(symbol, right), dot(dot) {};
		bool operator<(const ProductionItem &p) const;
		bool operator==(const ProductionItem &p) const;
	};

	// LR1产生式项目 (P -> A·b, a)
	struct ProductionLR1Item : ProductionItem {
		int search;			// search symbol (terminal)
		ProductionLR1Item(int symbol, vector<int>right, int dot, int search):
			ProductionItem(symbol, right, dot), search(search) {};

		bool operator<(const ProductionLR1Item &p) const;
		bool operator==(const ProductionLR1Item &p) const;

		// bool operator<(const ProductionLR1Item &p) const {
		// 	if (symbol < p.symbol) {
		// 		return true;
		// 	} else if (symbol == p.symbol) {
		// 		if (right < p.right) {
		// 			return true;
		// 		} else if (right == p.right) {
		// 			if (dot < p.dot) {
		// 				return true;
		// 			} else if (dot == p.dot) {
		// 				if (search < p.search) {
		// 					return true;
		// 				}
		// 			}
		// 		}
		// 	}
		// 	return false;
		// }

		// bool operator==(const ProductionLR1Item &p) const {
		// 	return (symbol == p.symbol && right	== p.right && dot == p.dot && search == p.search);
		// }
	};

	// lr(1) 分析表动作
	struct Action {
		enum TYPE {ACTION, REDUCE, GOTO, ACCEPT};
		TYPE type;	// 动作类型
		int tgt;	// ACTION or GOTO -> next_state; REDUCE -> prodution_idx
	};

	// 语法树节点
	struct GrammarNode {
		vector<GrammarNode *> children;
		int symbol;
	};

	inline int toNonterminal(int symbol)
	{
		return isNonterminal(symbol) ? symbol : symbol + CHARSET_SIZE;
	}
	inline int toTerminal(int symbol)
	{
		return isTerminal(symbol) ? symbol : symbol - CHARSET_SIZE;
	}

	inline bool isTerminal(int symbol)
	{
		return (0 <= symbol && symbol < CHARSET_SIZE);
	}

	inline bool isNonterminal(int symbol)
	{
		return (symbol >= CHARSET_SIZE);
	}

}


#endif
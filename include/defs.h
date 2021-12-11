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

namespace compiler {

const int ERROR_STATE =	-1;   	// dfa跳转错误状态, 作为dfa邻接矩阵的占位值
const int EMPTY_CHAR = 0; 		// 空字符, 作为nfa空边
const int END_CHAR = '#';		// 待分析字符串的结束字符, 作为语法分析终结标志
const int CHARSET_SIZE = 128;	// symbol is terminal <=> smaller than CHARSET_SIZE

inline int toNonterminal(int symbol);	// 普通字符(终结符)转符号(非终结符)
inline int toTerminal(int symbol);		// 符号(非终结符)转普通字符(终结符)
inline bool isTerminal(int symbol);		// 是否为终结符
inline bool isNonterminal(int symbol);	// 是否为非终结符

// since DFA is usually sparse, use key-value pair instead of matrix to store
typedef map<int, multimap<int, int>> NFAgraph;	// {from, {symbol, to}}
typedef map<int, map<int, int>>		 DFAgraph;	// {from, {symbol, to}}
struct DFA { DFAgraph graph; map<int, int> finality; };
struct NFA { NFAgraph graph; map<int, int> finality; };

struct Edge { int symbol; int from; int to; };
typedef vector<Edge> 				 EdgeTable; 

struct Token { int type; string val; };			// token

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

} // namespace compiler
#endif
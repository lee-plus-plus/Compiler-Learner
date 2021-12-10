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
		Production(symbol, right): 
			symbol(symbol), right(right) {};
		bool operator<(const Production &p);
		bool operator==(const Production &p);
	};

	// 产生式项目 (P -> A·b)
	struct ProductionItem : Production {
		int dot;			// position of dot (0 ~ right.size())
		ProductionItem(symbol, right, dot): 
			Production(symbol, right), dot(dot) {};
		bool operator<(const ProductionItem &p);
		bool operator==(const ProductionItem &p);
	};

	// LR1产生式项目 (P -> A·b, a)
	struct ProductionLR1Item : ProductionItem {
		int search;			// search symbol (terminal)
		ProductionLR1Item(symbol, right, dot, search):
			ProductionItem(symbol, right, dot), search(search) {};

		bool operator<(const ProductionLR1Item &p);
		bool operator==(const ProductionLR1Item &p);

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

}


#endif
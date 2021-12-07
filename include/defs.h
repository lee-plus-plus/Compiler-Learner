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

	const int ERROR_STATE = -1;   	// 错误链接
	const int EMPTY_CHAR = 0; 		// 空字符
	const int END_CHAR = '#';		// 结束字符
	const int CHARSET_SIZE = 128;	// dfa考虑字符集合

	// NFA邻接矩阵
	// e.g.: nfaEdges[32]['a'] = {31, 32}
	typedef array<vector<int>, CHARSET_SIZE> NFAitem; // 一行
	typedef vector<NFAitem> NFAedges;

	// DFA邻接矩阵
	// e.g.: dfaEdges[32]['a'] = 31
	// 默认state0为起始状态，所有运算均保证state0不错位
	typedef array<int, CHARSET_SIZE> DFAitem; // 一行
	typedef vector<DFAitem> DFAedges;
	typedef set<int> DFAfinality;

	// NFA/DFA邻接表
	struct Edge {
		int eChar;
		int from;
		int to;
	};
	typedef vector<Edge> EdgeTable;

	// token
	struct Token {
		int type;
		int st;
		int ed;
	};

	// 产生式
	struct Production {
		int symbol;		// above CHARSET_SIZE
		vector<int> right;	// mixed with symbols and characters

		bool operator<(const Production &p) const {
			if (symbol < p.symbol) {
				return true;
			} else if (symbol == p.symbol)
				if (right < p.right) {
					return true;
			}
			return false;
		}
	};

	// 的产生式项目
	struct ProductionItem {
		int symbol;		// above CHARSET_SIZE
		vector<int> right;	// mixed with symbols and characters
		int dot;		// position of dot

		bool operator<(const ProductionItem &p) const {
			if (symbol < p.symbol) {
				return true;
			} else if (symbol == p.symbol) {
				if (right < p.right) {
					return true;
				} else if (right == p.right) {
					if (dot < p.dot) {
						return true;
					}
				}
			}
			return false;
		}

		bool operator==(const ProductionItem &p) const {
			return (symbol == p.symbol && right	== p.right && dot == p.dot);
		}
	};

	struct ProductionLR1Item {
		int symbol;		// above CHARSET_SIZE
		vector<int> right;	// mixed with symbols and characters
		int dot;		// position of dot
		int search;		// search character

		bool operator<(const ProductionLR1Item &p) const {
			if (symbol < p.symbol) {
				return true;
			} else if (symbol == p.symbol) {
				if (right < p.right) {
					return true;
				} else if (right == p.right) {
					if (dot < p.dot) {
						return true;
					} else if (dot == p.dot) {
						if (search < p.search) {
							return true;
						}
					}
				}
			}
			return false;
		}

		bool operator==(const ProductionLR1Item &p) const {
			return (symbol == p.symbol && right	== p.right && dot == p.dot && search == p.search);
		}
	};

	// 是否为终结符(否则为非终结符)
	bool isTerminal(int charIdx);

	void printCovers(vector<set<int>> covers);
	void printNFA(NFAedges nfaEdges);
	void printDFA(DFAedges dfaEdges);
	void printDFA(DFAedges dfaEdges, set<int> dfaFinality);
	void printDFA(DFAedges dfaEdges, map<int, int> dfaFinalities);
	void printEdgeTable(EdgeTable edgeTable);
	void printSubStr(const string &src, int st, int ed);
	void printTokens(vector<Token> tokens, const string &src);
	void printSymbol(int charIdx);
	void printProduction(Production production);
	void printProductionItem(ProductionItem prodItem);
	void printProductionLR1Item(ProductionLR1Item prodItem);
	void printProductions(vector<Production> productions);
	


}


#endif
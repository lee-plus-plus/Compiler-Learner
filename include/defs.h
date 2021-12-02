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

#define ERR_LOG(...) {\
	fprintf(stderr, "%s:line %d:\t", __FILE__, __LINE__);	\
	fprintf(stderr, __VA_ARGS__);							\
	fprintf(stderr, "\n");									\
	throw "error";											\
}

namespace compiler {

	const int ERROR_STATE = -1;   	// 错误链接
	const int EMPTY_CHAR = 127; 	// 空字符
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
		string right;	// mixed with symbols and characters
	};

	// LR(1)的产生式项目
	struct ProductionItem {
		int symbol;		// above CHARSET_SIZE
		string right;	// mixed with symbols and characters
		int dot;		// position of dot
	};

	void printCovers(vector<set<int>> covers);
	void printNFA(NFAedges nfaEdges);
	void printDFA(DFAedges dfaEdges);
	void printDFA(DFAedges dfaEdges, set<int> dfaFinality);
	void printDFA(DFAedges dfaEdges, map<int, int> dfaFinalities);
	void printEdgeTable(EdgeTable edgeTable);
	void printStr(char *src, int st, int ed);
	void printTokens(vector<Token> tokens, char *src);
	


}


#endif
#ifndef SYNTAX_H
#define SYNTAX_H
#include "defs.h"
using namespace std;

namespace compiler {

	// NFA转DFA（未最小化）
	// return: dfa, dfaNode-nfaNodes映射关系
	pair<DFAgraph, multimap<int, int>> nfa2dfa(NFAgraph nfa, map<int, int> finality);
	// 最小化DFA
	pair<DFAgraph, map<int, int>> getMinimizedDfa(DFAgraph dfa, map<int, int> finality);

	// 消除DFA不可达状态, 并整理DFA状态数字至0 ~ numStates-1
	pair<DFAgraph, map<int, int>> getEmptyStatesEliminatedDfa(DFAgraph dfa, map<int, int> finality);
	// 合并DFA等价状态
	pair<DFAgraph, map<int, int>> getEqualStatesCombinedDfa(DFAgraph dfa, map<int, int> finality);

	// 扩张覆盖片选择（epsilon-闭包法）
	void setCoverExpanded(set<int> &cover, NFAgraph nfa);
	// 求后继覆盖片
	map<int, set<int>> getNextCovers(set<int> cover, NFAgraph nfa);
	
	// RE转NFA邻接表
	NFAgraph re2nfa(const string &re, int &numStates);

	void splitReByMid(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);
	void splitReByParen(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);

	// 将若干个DFA整合为1个大DFA
	pair<DFAgraph, map<int, int>> getDFAintegrated(vector<DFAgraph> dfas, vector<set<int>> dfaFinalities);

	// 获取tokens
	vector<Token> getTokens(DFAgraph dfa, map<int, int> finality, const string &src);

}



#endif

#ifndef NFA2DFA_H
#define NFA2DFA_H
#include "defs.h"
using namespace std;

namespace compiler {

	// 创建DFA
	DFAedges getNewDFAedges(int numStates, EdgeTable edgeTable = {});
	DFAedges getNewDFAedges(DFAedges slicedDFAwithHead);
	// 创建NFA
	NFAedges getNewNFAedges(int numStates, EdgeTable edgeTable = {});

	// NFA转DFA（未最小化）
	pair<DFAedges, set<int>> nfa2dfa(NFAedges nfa, set<int> nfaFinality);
	// DFA邻接矩阵转邻接表
	EdgeTable dfaEdges2EdgeTable(DFAedges dfa);
	// 最小化DFA
	pair<DFAedges, set<int>> getMinimizedDfa(DFAedges dfaEdges, set<int> dfaFinality);
	// 邻接表转NFA邻接矩阵
	NFAedges edgeTable2NFAedges(int numStates, EdgeTable edgeTable);

	// 消除DFA不可达状态
	pair<DFAedges, set<int>> getEmptyStatesEliminatedDfa(DFAedges dfaEdges, set<int> dfaFinality);
	// 合并DFA等价状态
	pair<DFAedges, set<int>> getEqualStatesCombinedDfa(DFAedges dfaEdges, set<int> dfaFinality);
	


	// 扩张覆盖片选择（epsilon-闭包法）
	void setCoverExpanded(set<int> &cover, NFAedges nfa);
	// 求后继覆盖片
	set<int> getNextCover(set<int> cover, NFAedges nfa, int charIdx);
	

	void testNfa2dfa();
	void testEliminateEmptyState();
	void testSimplifyEqualState();
}

#endif
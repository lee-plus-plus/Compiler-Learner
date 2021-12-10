#ifndef UTILS_H
#define UTILS_H
#include "defs.h"

namespace compiler {

	// 符号转字符串
	const char *symbolStr(int symbol);

	// 邻接表转NFA分析表
	NFAgraph toNFAgraph(EdgeTable edgeTable);
	// 邻接表转DFA分析表
	DFAgraph toDFAgraph(EdgeTable edgeTable);
	// DFA分析表转邻接表
	EdgeTable toEdgeTable(DFAgraph dfa);

	// 获取状态数量
	int getNumStates(DFAgraph dfa);

	void printCover(set<int> cover);
	void printEdgeTable(EdgeTable edgeTable);
	void printDFAgraph(DFAgraph dfa, map<int, int> finality);
	void printNFAgraph(NFAgraph nfa, map<int, int> finality);
	
	// void printProduction(Production production);
	// void printProductionItem(ProductionItem prodItem);
	// void printProductionLR1Item(ProductionLR1Item prodItem);
	// void printProductions(vector<Production> productions);
}


#endif
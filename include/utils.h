#ifndef UTILS_H
#define UTILS_H
#include "defs.h"

// terminal color
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
	void printDFA(DFA dfa);
	void printNFA(NFA nfa);
	
	// void printProduction(Production production);
	// void printProductionItem(ProductionItem prodItem);
	// void printProductionLR1Item(ProductionLR1Item prodItem);
	// void printProductions(vector<Production> productions);
}


#endif
#include "utils.h"
using namespace std;

namespace compiler {

	char *symbolStr(int symbol)
	{
		string result;
		if (isTerminal(symbol)) {
			result += symbol;
		} else {
			result += "[0;36m";
			result += toTerminal(symbol);
			result += "\e[0m";
		}
		return result.c_str();
	}

	// 邻接表转NFA分析表
	NFAtable toNFAtable(EdgeTable edgeTable)
	{
		NFAtable nfaTable;
		for (const Edge &edge : edgeTable) {
			if (nfaTable.count({{edge.from, edge.symbol}}) == 0) {
				nfaTable.insert({{edge.from, edge.symbol}, {edge.to}});
			} else {
				nfaTable[{edge.from, edge.symbol}].push_back(edge.to);
			}
		}
		return nfaTable;
	}

	// 邻接表转DFA分析表
	DFAtable toDFAtable(EdgeTable edgeTable)
	{
		DFAtable dfaTable;
		for (const Edge &edge : edgeTable) {
			dfaTable.insert({{edge.from, edge.symbol}, edge.to});
		}
		return dfaTable;
	}

	// DFA分析表转邻接表
	EdgeTable toEdgeTable(DFAgraph dfa)
	{
		EdgeTable edgeTable;
		for (const auto &node : dfa) {
			for (const autp &edge : node.second) {
				edgeTable.push_back({edge.first, node.first, edge.second});
			}
		}
		return edgeTable;
	}

	// 获取状态数量
	int getNumStates(DFAgraph dfa)
	{
		queue<int> q({0});
		set<int> visited({0});

		// bfs
		while (q.size()) {
			int state = q.front();
			q.pop();
			visited.insert(state);

			for (const auto &elem : dfa[state]) {
				int nextState = elem.second;
				// assign new id
				if (visited.count(nextState) == 0) {
					q.push(nextState);
					visited.insert(nextState);
				}
			}
		}

		return visited.size();
	}

	// void printProduction(Production prod)
	// {	
	// 	printSymbol(prod.symbol);
	// 	printf(" -> ");
	// 	for (int i = 0; i < prod.right.size(); i++) {
	// 		printSymbol(prod.right[i]);
	// 	}
	// 	printf("\n");
	// }

	// void printProductionItem(ProductionItem prodItem)
	// {	
	// 	printSymbol(prodItem.symbol);
	// 	printf(" -> ");
	// 	for (int i = 0; i < prodItem.right.size(); i++) {
	// 		if (i == prodItem.dot) {
	// 			printf("·");
	// 		}
	// 		printSymbol(prodItem.right[i]);
	// 	}
	// 	if (prodItem.dot == prodItem.right.size()) {
	// 		printf("·");
	// 	}
	// 	printf("\n");
	// }

	// void printProductionLR1Item(ProductionLR1Item prodItem)
	// {	
	// 	printSymbol(prodItem.symbol);
	// 	printf(" -> ");
	// 	for (int i = 0; i < prodItem.right.size(); i++) {
	// 		if (i == prodItem.dot) {
	// 			printf("·");
	// 		}
	// 		printSymbol(prodItem.right[i]);
	// 	}
	// 	if (prodItem.dot == prodItem.right.size()) {
	// 		printf("·");
	// 	}
	// 	printf(", ");
	// 	printSymbol(prodItem.search);
	// 	printf("\n");
	// }


	// void printProductions(vector<Production> productions)
	// {
	// 	printf("prod:{\n");
	// 	int i = 0;
	// 	for (Production prod : productions) {
	// 		printf("\t(%d) ", i++);
	// 		printProduction(prod);
	// 	}
	// 	printf("}\n");
	// }
}
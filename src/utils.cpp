#include "utils.h"
using namespace std;

namespace compiler {

	const char *symbolStr(int symbol)
	{
		string result;
		if (symbol == EMPTY_CHAR) {
			result += "ε";
		} else if (isTerminal(symbol)) {
			result += symbol;
		} else {
			result += T_BLUE;
			result += toTerminal(symbol);
			result += T_NONE;
		}
		return result.c_str();
	}

	// 邻接表转NFA分析表
	NFAgraph toNFAgraph(EdgeTable edgeTable)
	{
		NFAgraph nfa;
		for (const Edge &edge : edgeTable) {
			nfa[edge.from].insert({edge.symbol, edge.to});
		}
		return nfa;
	}


	// 邻接表转DFA分析表
	DFAgraph toDFAgraph(EdgeTable edgeTable)
	{
		DFAgraph dfa;
		for (const Edge &edge : edgeTable) {
			dfa[edge.from][edge.symbol] = edge.to;
		}
		return dfa;
	}

	// DFA分析表转邻接表
	EdgeTable toEdgeTable(DFAgraph dfa)
	{
		EdgeTable edgeTable;
		for (const auto &node : dfa) {
			for (const auto &edge : node.second) {
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

	void printCover(set<int> cover)
	{
		auto it = cover.begin();
		printf("{s%d", *it);
		for (it++; it != cover.end(); it++) {
			printf(", s%d", *it);
		}
		printf("}\n");
	}

	void printEdgeTable(EdgeTable edgeTable)
	{
		printf("edge table: \n");
		for (const Edge &edge : edgeTable) {
			printf("s%-2d --> %s --> s%-2d\n", edge.from, symbolStr(edge.symbol), edge.to);
		}
	}

	void printDFAgraph(DFAgraph dfa, map<int, int> finality)
	{
		set<int> symbolset;
		for (const auto &node : dfa) {
			for (const auto &edge : node.second) {
				symbolset.insert(edge.first);
			}
		}

		printf("DFA graph: \n");
		printf("\t");
		for (int symbol : symbolset) {
			printf("%s\t", symbolStr(symbol));
		}
		printf("\n");
		for (const auto &elem : finality) {
			const int &state = elem.first;
			printf("s%d:\t", state);
			for (int symbol : symbolset) {
				if (dfa[state].count(symbol)) {
					printf("s%d\t", dfa[state][symbol]);
				} else {
					printf("\t");
				}
			}
			if (finality[state] != 0) {
				printf(T_GREEN "*%d" T_NONE, finality[state]);
			}
			printf("\n");
		}
	}

	void printNFAgraph(NFAgraph nfa, map<int, int> finality)
	{
		set<int> symbolset;
		for (const auto &node : nfa) {
			for (const auto &edge : node.second) {
				symbolset.insert(edge.first);
			}
		}

		printf("NFA graph: \n");
		printf("\t");
		for (int symbol : symbolset) {
			printf("%s\t", symbolStr(symbol));
		}
		printf("\n");
		for (const auto &elem : finality) {
			const int &state = elem.first;
			printf("s%d:\t", state);
			for (int symbol : symbolset) {
				if (nfa[state].count(symbol)) {
					auto it = nfa[state].lower_bound(symbol);
					printf("s%d", it->second);
					for (it++; it != nfa[state].upper_bound(symbol); it++) {
						printf(",s%d", it->second);
					}
					printf("\t");
				} else {
					printf("\t");
				}
			}
			if (finality[state] != 0) {
				printf(T_GREEN "*%d" T_NONE, finality[state]);
			}
			printf("\n");
		}
	}

/*
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
	*/
}
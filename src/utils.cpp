#include "utils.h"
using namespace std;

namespace compiler {

string symbolStr(int symbol) {
	string result;
	if (symbol == EMPTY_CHAR) {
		result += "ε";
	} else if (isTerminal(symbol)) {
		result += symbol;
	} else {
		result += T_LIGHT_BLUE;
		result += toTerminal(symbol);
		result += T_NONE;
	}
	return result;
}

// 邻接表转NFA分析表
NFAgraph toNFAgraph(EdgeTable edgeTable) {
	NFAgraph nfa;
	for (const Edge &edge : edgeTable) {
		nfa[edge.from].insert({edge.symbol, edge.to});
	}
	return nfa;
}


// 邻接表转DFA分析表
DFAgraph toDFAgraph(EdgeTable edgeTable) {
	DFAgraph dfa;
	for (const Edge &edge : edgeTable) {
		dfa[edge.from][edge.symbol] = edge.to;
	}
	return dfa;
}

// DFA分析表转邻接表
EdgeTable toEdgeTable(DFAgraph dfa) {
	EdgeTable edgeTable;
	for (const auto &node : dfa) {
		for (const auto &edge : node.second) {
			edgeTable.push_back({edge.first, node.first, edge.second});
		}
	}
	return edgeTable;
}

// 比较人性化的语法初始化
// 例: {"A -> abc", "B -> Acd"} 
Grammar getGrammarFromStr(vector<string> prodStrs)
{
	set<int> symbolset;
	for (string prodStr : prodStrs) {
		symbolset.insert(prodStr[0]);
	}

	vector<Production> productions;
	for (string prodStr : prodStrs) {
		int symbol = toNonterminal(prodStr[0]);
		vector<int> right;
		for (int j = 5; j < prodStr.size(); j++) {
			// assume prodStr="A -> ...", right expression start from pos=5
			if (symbolset.count(prodStr[j]) != 0) {
				right.push_back(toNonterminal(prodStr[j]));
			} else {
				right.push_back(toTerminal(prodStr[j]));
			}
		}
		productions.push_back({symbol, right});
	}
	set<int> resSymbolset;
	for (int s : symbolset) {
		resSymbolset.insert(toNonterminal(s));
	}

	return Grammar({resSymbolset, productions});
}

// 获取状态数量
int getNumStates(DFAgraph dfa) {
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

void printCover(set<int> cover) {
	printf("{");
	if (cover.size() != 0) {
		auto it = cover.begin();
		printf("s%d", *it);
		for (it++; it != cover.end(); it++) {
			printf(", s%d", *it);
		}
	}
	printf("}\n");
}

void printEdgeTable(EdgeTable edgeTable) {
	printf("edge table: \n");
	for (const Edge &edge : edgeTable) {
		printf("s%-2d --> %s --> s%-2d\n", 
			   edge.from, symbolStr(edge.symbol).c_str(), edge.to);
	}
}

void printDFA(DFA dfa) {
	set<int> symbolset;
	for (const auto &node : dfa.graph) {
		for (const auto &edge : node.second) {
			symbolset.insert(edge.first);
		}
	}

	printf("DFA graph: \n");
	printf("\t");
	for (int symbol : symbolset) {
		printf("%s\t", symbolStr(symbol).c_str());
	}
	printf("\n");
	for (const auto &elem : dfa.finality) {
		const int &state = elem.first;
		printf("s%d:\t", state);
		for (int symbol : symbolset) {
			if (dfa.graph[state].count(symbol)) {
				printf("s%d\t", dfa.graph[state][symbol]);
			} else {
				printf("\t");
			}
		}
		if (dfa.finality[state] != 0) {
			printf(T_GREEN "*%d" T_NONE, dfa.finality[state]);
		}
		printf("\n");
	}
}

void printNFA(NFA nfa) {
	set<int> symbolset;
	for (const auto &node : nfa.graph) {
		for (const auto &edge : node.second) {
			symbolset.insert(edge.first);
		}
	}

	printf("NFA graph: \n");
	printf("\t");
	for (int symbol : symbolset) {
		printf("%s\t", symbolStr(symbol).c_str());
	}
	printf("\n");
	for (const auto &elem : nfa.finality) {
		const int &state = elem.first;
		printf("s%d:\t", state);
		for (int symbol : symbolset) {
			if (nfa.graph[state].count(symbol)) {
				auto it = nfa.graph[state].lower_bound(symbol);
				auto it_end = nfa.graph[state].upper_bound(symbol);
				printf("s%d", it->second);
				for (it++; it != it_end; it++) {
					printf(",s%d", it->second);
				}
				printf("\t");
			} else {
				printf("\t");
			}
		}
		if (nfa.finality[state] != 0) {
			printf(T_GREEN "*%d" T_NONE, nfa.finality[state]);
		}
		printf("\n");
	}
}

void printFirstset(const map<int, set<int>> &firstSet) {
	for (auto [symbol, nextSymbols] : firstSet) {
		if (isNonterminal(symbol)) {
			printf("%s: {", symbolStr(symbol).c_str());
			if (nextSymbols.size() != 0) {
				auto it = nextSymbols.begin();
				printf("%s", symbolStr(*it).c_str());
				for (it++; it != nextSymbols.end(); it++) {
					printf(", %s", symbolStr(*it).c_str());
				}
			}
			printf("} \n");
		}
	}
}

void printProduction(const Production &prod) {	
	printf("%s", symbolStr(prod.symbol).c_str());
	printf(" -> ");
	for (int i = 0; i < prod.right.size(); i++) {
		printf("%s", symbolStr(prod.right[i]).c_str());
	}
	printf("\n");
}

void printProductionItem(const ProductionItem &prodItem) {	
	printf("%s", symbolStr(prodItem.symbol).c_str());
	printf(" -> ");
	for (int i = 0; i < prodItem.right.size(); i++) {
		if (i == prodItem.dot) {
			printf("·");
		}
		printf("%s", symbolStr(prodItem.right[i]).c_str());
	}
	if (prodItem.dot == prodItem.right.size()) {
		printf("·");
	}
	printf("\n");
}

void printProductionLR1Item(const ProductionLR1Item &prodItem) {	
	printf("%s", symbolStr(prodItem.symbol).c_str());
	printf(" -> ");
	for (int i = 0; i < prodItem.right.size(); i++) {
		if (i == prodItem.dot) {
			printf("·");
		}
		printf("%s", symbolStr(prodItem.right[i]).c_str());
	}
	if (prodItem.dot == prodItem.right.size()) {
		printf("·");
	}
	printf(", ");
	printf("%s", symbolStr(prodItem.search).c_str());
	printf("\n");
}

void printGrammar(const Grammar &grammar) {
	printf("Grammar: \n");
	for (int i = 0; i < grammar.productions.size(); i++) {
		printf("(%d) ", i);
		printProduction(grammar.productions[i]);
	}
}

void printGrammarTreeNode(GrammarNode *node, vector<bool> isLast = {}) {
	if (isLast.size() == 0) {
		printf(" ");
	}
	for (int i = 0; i < isLast.size(); i++) {
		if (i + 1 == isLast.size()) {
			printf("%s", (isLast[i] ? " └─ " : " ├─ "));
		} else {
			printf("%s", (isLast[i] ? "   " : " │ "));
		}
	}
	printf("%s\n", symbolStr(node->symbol).c_str());
	isLast.push_back(false);
	for (auto it = node->children.begin(); 
			it != node->children.end(); it++) {
		if (it + 1 == node->children.end()) {
			isLast.back() = true;
		}
		printGrammarTreeNode(*it, isLast);
	}
}

void printGrammarTree(GrammarNode *root) {
	printf("Grammar Tree: \n");
	printGrammarTreeNode(root, {});
}

} // namespace compiler
#include "utils.h"
#include "lexical.h"
#include <iostream>
using namespace std;
using namespace compiler;

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

// // 测试首符集和随符集
void testFirstSetAndFollowSet()
{
	printf("test first-set and follow-set \n");
	printf("----------------------------- \n");
	// initialize symbolset and grammar
	Grammar grammar = getGrammarFromStr({
		"S -> A", 
		"A -> ABc", 
		"A -> Bbc", 
		"B -> Bc", 
	});
	printf("> initial grammar: \n");
	printGrammar(grammar);

	auto firstSet = getFirstSet(grammar);
	auto followSet = getFollowSet(grammar, firstSet);

	printf("> first-set: \n");
	printFirstset(firstSet);

	printf("> follow-set: \n");
	printFirstset(followSet);

	printf("\n");
}

void testLR1()
{
	printf("test LR(1) analysis \n");
	printf("------------------- \n");
	// initialize symbolset and grammar
	Grammar grammar = getGrammarFromStr({
		"S -> A", 
		"A -> ABc", 
		"A -> Bbc", 
		"B -> Bc", 
	});
	printGrammar(grammar);

	auto [covers, edgeTable] = getLR1dfa(grammar);
	printf("> covers of LR(1): \n");
	for (int i = 0; i < covers.size(); i++) {
		printf("c%d: \n", i);
		for (ProductionLR1Item prod : covers[i]) {
			printProductionLR1Item(prod);
		}
	}

	// print edge table
	printf("> edge table of LR(1) dfa: \n");
	printEdgeTable(edgeTable);

	map<pair<int, int>, Action> analyzeTable =  getLR1table(
		grammar.productions, covers, edgeTable
	);
	printf("> analysis table of LR(1) dfa: \n");
	for (auto [state_symbol, action] : analyzeTable) {
		
	}

	printf("\n");
}

// void dlrVisit(GrammarNode *node, int level = 0)
// {
// 	for (int i = 0; i < level; i++) {
// 		printf("  ");
// 	}
// 	printSymbol(node->symbol);
// 	printf("\n");
// 	for (GrammarNode *child : node->children) {
// 		dlrVisit(child, level + 1);
// 	}
// }

// void testLR1analyzeTable()
// {
// 	// initialize symbolset and grammar
// 	auto result = getNewGrammar({
// 		"Q -> E", 
// 		"E -> E+E", 
// 		"E -> E*E", 
// 		"E -> (E)", 
// 		"E -> i", 
// 	});
// 	set<int> symbolset = result.first;
// 	vector<Production> productions = result.second;
// 	printProductions(productions);

// 	auto result2 = getLR1dfa(symbolset, productions);
// 	vector<set<ProductionLR1Item>> covers = result2.first;
// 	EdgeTable edgeTable = result2.second;

// 	// print covers
// 	printf("covers: \n");
// 	for (int i = 0; i < covers.size(); i++) {
// 		printf("%d: \n", i);
// 		for (ProductionLR1Item prod : covers[i]) {
// 			printProductionLR1Item(prod);
// 		}
// 	}

// 	// print edge table
// 	printf("edge table: \n");
// 	for (Edge edge : edgeTable) {
// 		printf("%d ", edge.from);
// 		printSymbol(edge.eChar);
// 		printf(" %d\n", edge.to);
// 	}

// 	map<pair<int, int>, Action> analyzeTable;
// 	analyzeTable = getLR1table(productions, covers, edgeTable);

// 	string typeName[] = {"ACTION", "REDUCE", "GOTO  ", "ACCEPT"};
// 	printf("analyze table: \n");
// 	for (auto elem : analyzeTable) {
// 		printf("%2d ", elem.first.first);
// 		printSymbol(elem.first.second);
// 		printf(" %s %d\n", typeName[elem.second.type].c_str(), elem.second.tgt);
// 	}

// 	string src = "i*i+(i+i*i)";
// 	printf("input: \n");
// 	for (int i = 0; i < src.size(); i++) {
// 		printSymbol(src[i]);
// 	}
// 	printf("\n");

// 	GrammarNode *rootNode;
// 	printf("grammar tree:\n");
// 	rootNode = getLR1grammarTree(productions, analyzeTable, src);
// 	dlrVisit(rootNode);

// }

int main(int argc, char **argv)
{
	// testFirstSetAndFollowSet();
	testLR1();
	// testLR1analyzeTable();

	return 0;
	
}
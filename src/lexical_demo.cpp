#include "utils.h"
#include "lexical.h"
#include <iostream>
#include <string>
using namespace std;
using namespace compiler;

// // 测试首符集和随符集
void testFirstSetAndFollowSet()
{
	printf("test first-set and follow-set \n");
	printf("----------------------------- \n");
	// initialize symbolset and grammar
	Grammar grammar = getGrammarFromStr({
		"Q -> S", 
		"S -> V=R", 
		"S -> R", 
		"R -> L", 
		"L -> *R", 
		"L -> i", 
		"V -> a", 
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

// 测试LR1语法分析
void testLR1()
{
	printf("test LR(1) analysis \n");
	printf("------------------- \n");
	// initialize symbolset and grammar
	Grammar grammar = getGrammarFromStr({
		"Q -> S", 
		"S -> V=R", 
		"S -> R", 
		"R -> L", 
		"L -> *R", 
		"L -> i", 
		"V -> a", 
	});
	printGrammar(grammar);

	auto firstSet = getFirstSet(grammar);
	auto followSet = getFollowSet(grammar, firstSet);

	printf("> first-set: \n");
	printFirstset(firstSet);

	printf("> follow-set: \n");
	printFirstset(followSet);

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
	string typeName[] = {"ACTION", "REDUCE", "GOTO  ", "ACCEPT"};
	for (auto [key, action] : analyzeTable) {
		printf("s%d --> %s --> ", key.first, symbolStr(key.second).c_str());
		printf("%s ", typeName[action.type].c_str());
		if (action.type == Action::ACTION || 
			action.type == Action::GOTO) {
			printf("s%d", action.tgt);
		} else if (action.type == Action::REDUCE) {
			printf("r%d", action.tgt);
		}
		printf("\n");
	}

	printf("> use LR(1) analysis table to analyze: \n");
	printf("> input string: \n");
	string src = "a=*i";
	printf("%s\n", src.c_str());

	GrammarNode *root = getLR1grammarTree(
		grammar.productions, analyzeTable, src
	);
	printf("> grammar tree: \n");
	printGrammarTree(root);

	printf("\n");
}

int main(int argc, char **argv)
{
	// testFirstSetAndFollowSet();
	testLR1();

	return 0;
	
}
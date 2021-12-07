#include <iostream>
#include "grammar.h"
using namespace std;
using namespace compiler;

// 比较人性化的语法初始化
// 例: {"A -> abc", "B -> Acd"}
pair<set<int>, vector<Production>> getNewGrammar(vector<string> prodStrs)
{
	set<int> symbolset;
	vector<Production> productions;

	for (string prodStr : prodStrs) {
		symbolset.insert(prodStr[0]);
	}

	for (string prodStr : prodStrs) {
		Production prod;

		prod.symbol = prodStr[0] + CHARSET_SIZE;
		for (int j = 5; j < prodStr.size(); j++) {
			if (symbolset.count(prodStr[j]) != 0) {
				prod.right.push_back(prodStr[j] + CHARSET_SIZE);
			} else {
				prod.right.push_back(prodStr[j]);
			}
		}
		productions.push_back(prod);
	}

	for (int s : symbolset) {
		if (isTerminal(s)) {
			symbolset.erase(s);
			symbolset.insert(s + CHARSET_SIZE);
		}
	}

	return pair<set<int>, vector<Production>>(symbolset, productions);
}

void printFirstSet(map<int, set<int>> firstSet)
{
	for (auto elem : firstSet) {
		if (isTerminal(elem.first)) {
			continue;
		}
		printf("\t");
		printSymbol(elem.first);
		printf(": {");
		for (int c : elem.second) {
			printSymbol(c);
			printf(", ");
		}
		printf("}\n");
	}
}

// 测试首符集和随符集
void testFirstSetAndFollowSet()
{
	// initialize symbolset and grammar
	auto result = getNewGrammar({
		"S -> A", 
		"A -> ABc", 
		"A -> Bbc", 
		"B -> Bc", 
	});
	set<int> symbolset = result.first;
	vector<Production> productions = result.second;
	printProductions(productions);

	auto firstSet = getFirstSet(symbolset, productions);
	auto followSet = getFollowSet(symbolset, productions, firstSet);

	printf("firstset: \n");
	printFirstSet(firstSet);
	printf("followset: \n");
	printFirstSet(followSet);
}

void testLR0()
{
	// initialize symbolset and grammar
	auto result = getNewGrammar({
		"S -> A", 
		"A -> ABc", 
		"A -> Bbc", 
		"B -> Bc", 
	});
	set<int> symbolset = result.first;
	vector<Production> productions = result.second;
	printProductions(productions);

	auto result2 = getLR0dfa(symbolset, productions);
	vector<set<ProductionItem>> covers = result2.first;
	EdgeTable edgeTable = result2.second;

	// print covers
	printf("covers: \n");
	for (int i = 0; i < covers.size(); i++) {
		printf("%d: \n", i);
		for (ProductionItem prod : covers[i]) {
			printProductionItem(prod);
		}
	}

	// print edge table
	printf("edgetable: \n");
	for (Edge edge : edgeTable) {
		printf("%d ", edge.from);
		printSymbol(edge.eChar);
		printf(" %d\n", edge.to);
	}
}

void testLR1()
{
	// initialize symbolset and grammar
	auto result = getNewGrammar({
		"S -> A", 
		"A -> ABc", 
		"A -> Bbc", 
		"B -> Bc", 
	});
	set<int> symbolset = result.first;
	vector<Production> productions = result.second;
	printProductions(productions);

	auto result2 = getLR1dfa(symbolset, productions);
	vector<set<ProductionLR1Item>> covers = result2.first;
	EdgeTable edgeTable = result2.second;

	// print covers
	printf("covers: \n");
	for (int i = 0; i < covers.size(); i++) {
		printf("%d: \n", i);
		for (ProductionLR1Item prod : covers[i]) {
			printProductionLR1Item(prod);
		}
	}

	// print edge table
	printf("edgetable: \n");
	for (Edge edge : edgeTable) {
		printf("%d ", edge.from);
		printSymbol(edge.eChar);
		printf(" %d\n", edge.to);
	}
}

int main(int argc, char **argv)
{
	// testFirstSetAndFollowSet();
	testLR1();

	return 0;
	
}
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "defs.h"
using namespace std;

namespace compiler {
	// 求首符集
	map<int, set<int>> getFirstSet(set<int> symbolset, vector<Production> productions);
	// 求随符集
	map<int, set<int>> getFollowSet(set<int> symbolset, vector<Production> productions, map<int, set<int>> firstSet);

	// lr(0)分析
	pair<vector<set<ProductionItem>>, EdgeTable> getLR0dfa(set<int> symbolset, vector<Production> prods);
	void setLR0CoverExpanded(set<ProductionItem> &cover, vector<Production> prods);

	// lr(1)分析
	pair<vector<set<ProductionLR1Item>>, EdgeTable> getLR1dfa(set<int> symbolset, vector<Production> prods);
	void setLR1CoverExpanded(set<ProductionLR1Item> &cover, map<int, set<int>> firstSet, vector<Production> prods);

	pair<vector<Production>, map<int, int>> getLeftRecursiveReducedGrammar();

	// LR(1) analyze table
	map<pair<int, int>, Action> getLR1table(vector<Production> prods, vector<set<ProductionLR1Item>> covers, EdgeTable edgeTable);

	// LR(1) analyze, return grammar tree
	GrammarNode *getLR1grammarTree(vector<Production> prods, map<pair<int, int>, Action> analyzeTable, string src);

};

#endif
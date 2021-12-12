#ifndef LEXICAL_H
#define LEXICAL_H
#include "defs.h"
using namespace std;

namespace compiler {

// 求非终结符的首符集
// 输入非终结符集合和CFG文法, 返回首符集
map<int, set<int>> getFirstSet(Grammar grammar);

// 求非终结符的随符集
// 输入非终结符集合和CFG文法, 返回随符集
map<int, set<int>> getFollowSet(Grammar grammar, map<int, set<int>> firstSet);

// lr(0)分析
// 输入非终结符集合和CFG文法, 返回nfa(lr(0)覆盖片映射和nfa邻接表)
// pair<vector<set<ProductionItem>>, EdgeTable> getLR0dfa(set<int> symbolset, 
// 												vector<Production> prods);

// void setLR0CoverExpanded(set<ProductionItem> &cover, vector<Production> prods);

// lr(1)分析
// 输入非终结符集合和CFG文法, 返回nfa(lr(0)覆盖片映射和nfa邻接表)
pair<vector<LR1Cover>, EdgeTable> getLR1dfa(Grammar grammar);

void setLR1CoverExpanded(LR1Cover &cover, map<int, set<int>> firstSet, 
						 vector<Production> prods);

// pair<vector<Production>, map<int, int>> getLeftRecursiveReducedGrammar();

// LR(1) analyze table
map<pair<int, int>, Action> getLR1table(vector<Production> prods, 
										vector<LR1Cover> covers, 
										EdgeTable edgeTable);

// LR(1) analyze, return grammar tree
GrammarNode *getLR1grammarTree(vector<Production> prods, 
							   map<pair<int, int>, Action> analyzeTable, 
							   string src);

}; // namespace compiler

#endif
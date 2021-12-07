#ifndef SYNTAX_H
#define SYNTAX_H
#include "defs.h"
#include "nfa2dfa.h"
using namespace std;

namespace compiler {

	// 将若干个DFA整合为1个大DFA
	pair<DFAedges, map<int, int>> getDFAintegrated(vector<DFAedges> dfas, vector<set<int>> dfaFinalities);

	// 获取tokens
	vector<Token> getTokens(DFAedges dfa, map<int, int> dfaFinalities, const string &src);

}



#endif

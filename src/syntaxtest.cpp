#include <iostream>
#include <string>
// #include "nfa2dfa.h"
// #include "re2nfa.h"
#include "syntax.h"
using namespace std;
using namespace compiler;


void testSyntaxAnalyze()
{
	const int N = 3;
	// 输入N个正则表达式
	string re[N];

	for (int i = 0; i < N; i++) {
		printf("Regular Expression %d: ", i);
		getline(cin, re[i]);
		printf("RE[%d] == \"%s\"\n", i, re[i].c_str());
	}

	// 正则表达式转DFA
	vector<DFAedges> dfas(N);
	vector<set<int>> dfaFinalities(N);
	for (int i = 0; i < N; i++) {
		int numStates;
		// rez转nfa
		EdgeTable edgeTable = re2nfa(re[i], re[i].size(), numStates);
		NFAedges nfaEdges = edgeTable2NFAedges(numStates, edgeTable);
		// nfa转dfa
		auto result = nfa2dfa(nfaEdges, {1});
		result = getMinimizedDfa(result.first, result.second);

		dfas[i] = result.first;
		dfaFinalities[i] = result.second;

		printf("DFA of regular Expression %d:\n", i);
		printDFA(dfas[i], dfaFinalities[i]);
	}

	// DFA合并
	auto result = getDFAintegrated(dfas, dfaFinalities);
	DFAedges dfai = result.first;
	map<int, int> dfaiFinities = result.second;
	
	printf("DFA integrated:\n");
	printDFA(dfai, dfaiFinities);

	// 用合并后的DFA提取字符串tokens
	printf("Get tokens:\n");
	while (true) {
		string src;
		getline(cin, src);

		vector<Token> tokens;
		tokens = getTokens(dfai, dfaiFinities, src);

		printTokens(tokens, src);	
	}
	
}

int main()
{
	testSyntaxAnalyze();
}
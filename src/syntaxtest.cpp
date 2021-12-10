#include <iostream>
#include <string>
#include "utils.h"
#include "syntax.h"
using namespace std;
using namespace compiler;

// void testSyntaxAnalyze()
// {
// 	const int N = 3;
// 	// 输入N个正则表达式
// 	string re[N];

// 	for (int i = 0; i < N; i++) {
// 		printf("Regular Expression %d: ", i);
// 		getline(cin, re[i]);
// 		printf("RE[%d] == \"%s\"\n", i, re[i].c_str());
// 	}

// 	// 正则表达式转DFA
// 	vector<DFAedges> dfas(N);
// 	vector<set<int>> dfaFinalities(N);
// 	for (int i = 0; i < N; i++) {
// 		int numStates;
// 		// rez转nfa
// 		EdgeTable edgeTable = re2nfa(re[i], re[i].size(), numStates);
// 		NFAedges nfaEdges = edgeTable2NFAedges(numStates, edgeTable);
// 		// nfa转dfa
// 		auto result = getNfa2Dfa(nfaEdges, {1});
// 		result = getMinimizedDfa(result.first, result.second);

// 		dfas[i] = result.first;
// 		dfaFinalities[i] = result.second;

// 		printf("DFA of regular Expression %d:\n", i);
// 		printDFA(dfas[i], dfaFinalities[i]);
// 	}

// 	// DFA合并
// 	auto result = getDFAintegrated(dfas, dfaFinalities);
// 	DFAedges dfai = result.first;
// 	map<int, int> dfaiFinities = result.second;
	
// 	printf("DFA integrated:\n");
// 	printDFA(dfai, dfaiFinities);

// 	// 用合并后的DFA提取字符串tokens
// 	printf("Get tokens:\n");
// 	while (true) {
// 		string src;
// 		getline(cin, src);

// 		vector<Token> tokens;
// 		tokens = getTokens(dfai, dfaiFinities, src);

// 		printTokens(tokens, src);	
// 	}
	
// }

void testDFAminimized()
{
	printf("test for minimize DFA\n");
	printf("---------------------\n");

	DFAgraph dfa({
		{ 0,	{{'a', 1},	{'b', 2}} }, 
		{ 1,	{{'a', 1},	{'b', 3}} }, 
		{ 2, 	{{'a', 1},	{'b', 2}} }, 
		{ 3, 	{{'a', 1},	{'b', 2}} }, 
		{ 4, 	{{'a', 4}} }, 
	});
	map<int, int> finality({
		{0, 0}, {1, 0}, {2, 0}, {3, 1}, {4, 0}
	});
	printf("> initial DFA: \n");
	printDFAgraph(dfa, finality);

	auto result1 = getMergedDfa(dfa, finality);
	DFAgraph dfa1 = result1.first;
	map<int, int> finality1 = result1.second;
	printf("> merge equal states: \n");
	printDFAgraph(dfa1, finality1);

	auto result2 = getReachableDfa(dfa1, finality1);
	DFAgraph dfa2 = result2.first;
	map<int, int> finality2 = result2.second;	
	printf("> eliminate unreachable states: \n");
	printDFAgraph(dfa2, finality2);

	printf("\n");
}


void testDFAtoNFA()
{
	printf("test for DFA to NFA\n");
	printf("-------------------\n");

	NFAgraph nfa({
		{ 0,	{{'\0', 1}, {'\0', 7}} }, 
		{ 1,	{{'\0', 2}, {'\0', 4}} }, 
		{ 2, 	{{'a',  3}} }, 
		{ 3, 	{{'\0', 6}} }, 
		{ 4, 	{{'b',  5}} }, 
		{ 5, 	{{'\0', 6}} }, 
		{ 6, 	{{'\0', 7}} }, 
		{ 7, 	{{'a',  8}} }, 
		{ 8, 	{{'b',  9}} }, 
	});
	map<int, int> finality({
		{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, 
		{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 1}, 
	});
	printf("> initial NFA: \n");
	printNFAgraph(nfa, finality);

	auto result1 = getNfa2DfaCoverMap(nfa, finality);
	DFAgraph dfa1 = result1.first;
	map<int, set<int>> coverMap = result1.second;
	map<int, int> finality1 = getFinalityFromCoverMap(finality, coverMap);
	printf("> generate covers of NFA states: \n");
	printf("covers: \n");
	for (int i = 0; i < coverMap.size(); i++) {
		printf("s%d: ", i);
		printCover(coverMap[i]);
	}
	printDFAgraph(dfa1, finality1);
	
	auto result2 = getMinimizedDfa(dfa1, finality1);
	DFAgraph dfa2 = result2.first;
	map<int, int> finality2 = result2.second;
	printf("> NFA to DFA: \n");	
	printDFAgraph(dfa2, finality2);

	printf("\n");
}

void testREtoNFA()
{
	printf("test for RE to NFA\n");
	printf("------------------\n");

	string re = "(a|b)+c";
	printf("Regular Expression: %s\n", re.c_str());

	auto result1 = re2nfa(re);
	NFAgraph nfa1 = result1.first;
	map<int, int> finality1 = result1.second;
	printf("> generate NFA from RE: \n");
	printNFAgraph(nfa1, finality1);

	auto result2 = getNfa2Dfa(nfa1, finality1);
	result2 = getMinimizedDfa(result2.first, result2.second);
	DFAgraph dfa2 = result2.first;
	map<int, int> finality2 = result2.second;
	printf("> minimize NFA to DFA: \n");
	printDFAgraph(dfa2, finality2);

	printf("\n");
}

int main(int argc, char **argv)
{
	testDFAminimized();
	testDFAtoNFA();
	testREtoNFA();
	
	return 0;
}
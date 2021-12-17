#include "utils.h"
#include "syntax.h"
#include <iostream>
#include <cstring>
#include <string>
using namespace std;
using namespace compiler;

void testDFAminimized()
{
	printf("test for minimize DFA\n");
	printf("---------------------\n");

	DFA dfa = {{
		{ 0,	{{'a', 1},	{'b', 2}} }, 
		{ 1,	{{'a', 1},	{'b', 3}} }, 
		{ 2, 	{{'a', 1},	{'b', 2}} }, 
		{ 3, 	{{'a', 1},	{'b', 2}} }, 
		{ 4, 	{{'a', 4}} }, 
	}, {
		{0, 0}, {1, 0}, {2, 0}, {3, 1}, {4, 0}
	}};
	printf("> initial DFA: \n");
	printDFA(dfa);

	dfa = getMergedDfa(dfa);
	printf("> merge equal states: \n");
	printDFA(dfa);

	dfa = getReachableDfa(dfa);
	printf("> eliminate unreachable states: \n");
	printDFA(dfa);

	printf("\n");
}

void testNFAtoDFA()
{
	printf("test for DFA to NFA\n");
	printf("-------------------\n");

	NFA nfa = {{
		{ 0,	{{'\0', 1}, {'\0', 7}} }, 
		{ 1,	{{'\0', 2}, {'\0', 4}} }, 
		{ 2, 	{{'a',  3}} }, 
		{ 3, 	{{'\0', 6}} }, 
		{ 4, 	{{'b',  5}} }, 
		{ 5, 	{{'\0', 6}} }, 
		{ 6, 	{{'\0', 7}} }, 
		{ 7, 	{{'a',  8}} }, 
		{ 8, 	{{'b',  9}} }, 
	}, {
		{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, 
		{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 1}, 
	}};
	printf("> initial NFA: \n");
	printNFA(nfa);

	// use structured binding in C++17
	auto [dfaGraph, coverMap] = getCoverMapfromNFAgraph(nfa.graph); 
	map<int, int> dfaFinality = getFinalityFromCoverMap(nfa.finality, coverMap);
	DFA dfa = {dfaGraph, dfaFinality};
	printf("> generate covers of NFA states: \n");
	printf("> state in dfa represent these covers: \n");
	printf("> covers: \n");
	for (int i = 0; i < coverMap.size(); i++) {
		printf("c%d: ", i);
		printCover(coverMap[i]);
	}
	printDFA(dfa);
	
	dfa = getMinimizedDfa(dfa);
	printf("> NFA to DFA: \n");	
	printDFA(dfa);

	printf("\n");
}

void testREtoDFA()
{
	printf("test for RE to NFA\n");
	printf("------------------\n");

	string re = "(a|b)+c";
	printf("Regular Expression: %s\n", re.c_str());

	NFA nfa = getNFAfromRE(re);
	printf("> generate NFA from RE: \n");
	printNFA(nfa);

	DFA dfa = getDFAfromNFA(nfa);
	dfa = getMinimizedDfa(dfa);
	printf("> minimize NFA to DFA: \n");
	printDFA(dfa);

	printf("\n");
}

void testSyntaxAnalyze()
{
	printf("test for multiple REs to DFA\n");
	printf("----------------------------\n");

	vector<string> re = {
		"_+", 
		"(a|b|c)(a|b|c|1|2|3)*", 
		"(1|2|3)(1|2|3|0)*"
	};
	printf("> given regular expressions: \n");
	for (int i = 0; i < re.size(); i++) {
		printf("RE %d: %s\n", i + 1, re[i].c_str());
	}

	vector<DFA> dfas(re.size());
	for (int i = 0; i < re.size(); i++) {
		dfas[i] = getMinimizedDfa(getDFAfromNFA(getNFAfromRE(re[i])));
		for (auto it = dfas[i].finality.begin(); 
				it != dfas[i].finality.end(); it++) {
			if (it->second != 0) {
				it->second = i + 1;
			}
		}
	}
	printf("> generate DFAs from REs: \n");
	for (int i = 0; i < 3; i++) {
		printf("> DFA from RE %d: \n", i + 1);
		printDFA(dfas[i]);
	}

	DFA dfai = getDFAintegrated(dfas);
	printf("> integrated DFA: \n");
	printDFA(dfai);
	
	string src = "a1___23___30__a1____";
	printf("> input string: \n");
	printf("%s\n", src.c_str());
	vector<Token> tokens = getTokens(dfai, src);
	printf("> tokens: \n");
	for (const Token &token : tokens) {
		printf("[type: %d, val: '%s']\n", token.type, token.val.c_str());
	}

	// printf("\n");
}

int main(int argc, char **argv)
{
	testDFAminimized();
	testNFAtoDFA();
	testREtoDFA();
	testSyntaxAnalyze();
	// reInteractiveTest();
	
	return 0;
}
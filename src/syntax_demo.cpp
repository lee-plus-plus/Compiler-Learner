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

void testMultiREtoDFA()
{
	printf("test for multiple REs to DFA\n");
	printf("----------------------------\n");

	string re[3] = {"b(a|b)+bc", "aab+", "ac(ca)*a"};
	printf("> given regular expressions: \n");
	for (int i = 0; i < 3; i++) {
		printf("RE %d: %s\n", i + 1, re[i].c_str());
	}

	auto res10 = re2nfa(re[0]);
	auto res11 = re2nfa(re[1]);
	auto res12 = re2nfa(re[2]);
	NFAgraph nfa1[3] = {res10.first, res11.first, res12.first};
	map<int, int> finality1[3] = {res10.second, res11.second, res12.second};
	auto res20 = getNfa2Dfa(nfa1[0], finality1[0]);
	auto res21 = getNfa2Dfa(nfa1[1], finality1[1]);
	auto res22 = getNfa2Dfa(nfa1[2], finality1[2]);
	vector<DFAgraph> dfa2 = {res20.first, res21.first, res22.first};
	vector<map<int, int>> finality2 = {res20.second, res21.second, res22.second};
	for (int i = 0; i < 3; i++) {
		for (auto it = finality2[i].begin(); it != finality2[i].end(); it++) {
			if (it->second != 0) {
				it->second = i + 1;
			}
		}
	}
	printf("> generate DFAs from REs: \n");
	for (int i = 0; i < 3; i++) {
		printf("> DFA from RE %d: \n", i + 1);
		printDFAgraph(dfa2[i], finality2[i]);
	}
	
	auto result3 = getDFAintegrated(dfa2, finality2);
	DFAgraph dfa3 = result3.first;
	map<int, int> finality3 = result3.second;
	printf("> integrated DFA: \n");
	printDFAgraph(dfa3, finality3);

	printf("\n");
}

void testSyntaxAnalyze()
{
	printf("test for multiple REs to DFA\n");
	printf("----------------------------\n");

	vector<string> re = {
		".+", 
		"(a|b|c)(a|b|c|1|2|3)*", 
		"(1|2|3)(1|2|3|0)*"
	};
	printf("> given regular expressions: \n");
	for (int i = 0; i < re.size(); i++) {
		printf("RE %d: %s\n", i + 1, re[i].c_str());
	}


	vector<pair<NFAgraph, map<int, int>>> res1(re.size());
	for (int i = 0; i < re.size(); i++) {
		res1[i] = re2nfa(re[i]);
	}

	vector<pair<DFAgraph, map<int, int>>> res2(re.size());
	for (int i = 0; i < re.size(); i++) {
		res2[i] = getNfa2Dfa(res1[i].first, res1[i].second);
		auto &finality = res2[i].second;
		for (auto it = finality.begin(); it != finality.end(); it++) {
			if (it->second != 0) {
				it->second = i + 1;
			}
		}
	}

	vector<DFAgraph> dfa(re.size());
	vector<map<int, int>> finality(re.size());
	for (int i = 0; i < re.size(); i++) {
		dfa[i] = res2[i].first;
		finality[i] = res2[i].second;
	}
	auto result3 = getDFAintegrated(dfa, finality);
	DFAgraph dfa3 = result3.first;
	map<int, int> finality3 = result3.second;
	printf("> generate DFA: \n");
	printDFAgraph(dfa3, finality3);
	
	string src = "a1...23...30..a1....";
	printf("> input string: \n");
	printf("%s\n", src.c_str());
	vector<Token> tokens = getTokens(dfa3, finality3, src);
	printf("> tokens: \n");
	for (const Token &token : tokens) {
		printf("[type: %d, val: '%s']\n", token.type, token.val.c_str());
	}

	// printf("\n");
}

int main(int argc, char **argv)
{
	// testDFAminimized();
	// testDFAtoNFA();
	// testREtoNFA();
	// testMultiREtoDFA();
	testSyntaxAnalyze();
	
	return 0;
}
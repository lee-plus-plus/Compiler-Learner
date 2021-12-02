#include <cstdio>
#include <stack>
#include <iostream>
#include <cstring>
#include <vector>
#include <cassert>
#include "re2nfa.h"
using namespace std;

namespace compiler {
	// RE转NFA邻接表
	EdgeTable re2nfa(const string &re, int length, int &numStates)
	{
		numStates = 2;
		EdgeTable nfa;
		splitReByMid(
			re, 0, length,  
			numStates, nfa, 0, 1
		);

		return nfa;
	}

	// 提取'|'并联
	void splitReByMid(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState)
	{
		int parenLevel = 0;
		int midPos = st - 1;
		for (int i = st; i < ed; i++) {
			switch (re[i]) {
				case '|':
					if (parenLevel != 0) {
						break;
					}
					splitReByParen(
						re, midPos + 1, i, 
						numStates, edges, stState, edState
					);
					midPos = i;
					break;
				case '(':
					parenLevel++;
					break;
				case ')':
					parenLevel--;
					if (parenLevel < 0) {
						ERR_LOG("syntax error: mismatched parenthesis ')'");
					}
					break;
				case '\\':
					if (i + 1 >= ed) {
						ERR_LOG("syntax error: separate escape character '\\'");
					}
					i++;
					// no break
				default:
					break;
			}
		}
		if (parenLevel != 0) {
			ERR_LOG("syntax error: unmatched parenthesis '('");
		}
		splitReByParen(
			re, midPos + 1, ed, 
			numStates, edges, stState, edState
		);
	}

	// 提取'('')'串联
	void splitReByParen(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState)
	{
		int parenLevel = 0;
		int lparenPos = -1;
		int lparenState = stState;
		int edState2 = numStates++;
		for (int i = st; i < ed; i++) {
			switch (re[i]) {
				case '(':
					if (parenLevel == 0) {
						lparenPos = i;
						lparenState = stState;
					}
					parenLevel++;
					break;
				case ')': 
					parenLevel--; 
					if (parenLevel < 0) {
						ERR_LOG("syntax error: mismatched parenthesis ')'");
					}
					
					if (parenLevel == 0) {
						if (i + 1 < ed && re[i + 1] == '*') {
							splitReByMid(
								re, lparenPos + 1, i, 
								numStates, edges, stState, stState
							);
							i++;
						} else if (i + 1 < ed && re[i + 1] == '+') {
							splitReByMid(
								re, lparenPos + 1, i, 
								numStates, edges, stState, stState
							);
							splitReByMid(
								re, lparenPos + 1, i, 
								numStates, edges, stState, edState2
							);
							stState = edState2;
							edState2 = numStates++;
							i++;
						} else {
							splitReByMid(
								re, lparenPos + 1, i, 
								numStates, edges, stState, edState2
							);
							stState = edState2;
							edState2 = numStates++;						
						}
					}
					break;
				case '|': 
					assert(parenLevel > 0);
					break;
				case '*': 
					ERR_LOG("syntax error: mismatched '*'");
					break;
				case '+':
					ERR_LOG("syntax error: mismatched '+'");
					break;
				case '\\':
					if (i + 1 >= ed) {
						ERR_LOG("syntax error: separate escape character '\\'");
					}
					i++;
					// no break
				default:
					if (parenLevel == 0) {
						if (i + 1 < ed && re[i + 1] == '*') {
							edges.push_back({re[i], stState, stState});
							i++;
						} else if (i + 1 < ed && re[i + 1] == '+') {
							edges.push_back({re[i], stState, stState});
							edges.push_back({re[i], stState, edState2});
							stState = edState2;
							edState2 = numStates++;
							i++;
						} else {
							edges.push_back({re[i], stState, edState2});
							stState = edState2;
							edState2 = numStates++;						
						}
					}
					break;
				
			}
		}
		if (parenLevel != 0) {
			ERR_LOG("syntax error: unmatched parenthesis '('");
		}
		edges.push_back({EMPTY_CHAR, stState, edState});
	}

	void testRe2Nfa()
	{
		char re[] = "(abc)+ab";
		int numStates;
		
		// rez转nfa
		EdgeTable edgeTable = re2nfa(re, strlen(re), numStates);
		// NFAedges nfaEdges = edgeTable2NFAedges(numStates, edgeTable);
		// // nfa转dfa
		// auto result = nfa2dfa(nfaEdges, {1});
		// result = getMinimizedDfa(result.first, result.second);

		// DFAedges dfaEdges = result.first;
		// set<int> dfaFinality = result.second;

		printEdgeTable(edgeTable);
		// printNFA(nfaEdges);
		// printDFA(dfaEdges);
		// for (int i : dfaFinality) {
		// 	printf("%d ", i);
		// }
		// printf("\n");
	}
}

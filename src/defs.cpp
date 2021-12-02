#include "defs.h"
using namespace std;

namespace compiler {

	void printCovers(vector<set<int>> covers)
	{
		printf("covers:{ \n");
		for (set<int> elem : covers) {
			printf("\t{ ");
			for (int i : elem) {
				printf("%d, ", i);
			}
			printf(" }, \n");
		}
		printf(" }\n");
	}

	void printNFA(NFAedges nfa)
	{
		bool isUseful[CHARSET_SIZE];
		for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
			isUseful[charIdx] = false;
			for (NFAitem item : nfa) {
				for (int i : item[charIdx]) {
					if (i != ERROR_STATE) {
						isUseful[charIdx] = true;
						break;
					}
				}
			}
		}

		printf("nfa:{\t");
		for (int i = 0; i < CHARSET_SIZE; i++) {
			if (!isUseful[i]) {
				continue;
			}
			printf(" '%c',\t", i);
		}
		printf(" }\n");
		for (int i = 0; (uint32_t)i < nfa.size(); i++) {
			printf("%2d:{\t", i);
			for (int j = 0; j < CHARSET_SIZE; j++) {
				if (!isUseful[j]) {
					continue;
				}
				printf("{");
				for (int k = 0; (uint32_t)k < nfa[i][j].size(); k++) {
					if (nfa[i][j][k] != ERROR_STATE) {
						printf("%d ", nfa[i][j][k]);
					} else {
						printf("  ");
					}
				}
				printf("}\t");
			}
			printf(" }\n");
		}
	}

	void printDFA(DFAedges dfaEdges)
	{
		bool isUseful[CHARSET_SIZE];
		for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
			isUseful[charIdx] = false;
			for (DFAitem item : dfaEdges) {
				if (item[charIdx] != ERROR_STATE) {
					isUseful[charIdx] = true;
					break;
				}
			}
		}

		printf("dfa: {\t");
		for (int i = 1; i < CHARSET_SIZE; i++) {
			if (!isUseful[i]) {
				continue;
			}
			printf("' %c',\t", i);
		}
		printf(" }\n");
		for (int i = 0; (uint32_t)i < dfaEdges.size(); i++) {
			printf("%2d :{\t", i);
			for (int c = 0; c < CHARSET_SIZE; c++) {
				if (!isUseful[c]) {
					continue;
				}
				printf("{");
				if (dfaEdges[i][c] != ERROR_STATE) {
					printf("%2d ", dfaEdges[i][c]);
				} else {
					printf("   ");
				}
				printf("},\t");
			}
			printf(" }\n");
		}
	}

	void printDFA(DFAedges dfaEdges, set<int> dfaFinality)
	{
		bool isUseful[CHARSET_SIZE];
		for (int c = 0; c < CHARSET_SIZE; c++) {
			isUseful[c] = false;
			for (DFAitem item : dfaEdges) {
				if (item[c] != ERROR_STATE) {
					isUseful[c] = true;
					break;
				}
			}
		}

		printf("dfa: {\t");
		for (int i = 1; i < CHARSET_SIZE; i++) {
			if (!isUseful[i]) {
				continue;
			}
			printf("' %c',\t", i);
		}
		printf(" }\n");
		for (int i = 0; (uint32_t)i < dfaEdges.size(); i++) {
			printf("%2d%c:{\t", i, dfaFinality.count(i) ? '*' : ' ');
			for (int c = 0; c < CHARSET_SIZE; c++) {
				if (!isUseful[c]) {
					continue;
				}
				printf("{");
				if (dfaEdges[i][c] != ERROR_STATE) {
					printf("%2d ", dfaEdges[i][c]);
				} else {
					printf("   ");
				}
				printf("},\t");
			}
			printf(" }\n");
		}
	}

	void printDFA(DFAedges dfaEdges, map<int, int> dfaFinalities)
	{
		bool isUseful[CHARSET_SIZE];
		for (int c = 0; c < CHARSET_SIZE; c++) {
			isUseful[c] = false;
			for (DFAitem item : dfaEdges) {
				if (item[c] != ERROR_STATE) {
					isUseful[c] = true;
					break;
				}
			}
		}

		printf("dfa: {\t");
		for (int i = 1; i < CHARSET_SIZE; i++) {
			if (!isUseful[i]) {
				continue;
			}
			printf("' %c',\t", i);
		}
		printf(" }\n");
		for (int i = 0; (uint32_t)i < dfaEdges.size(); i++) {
			if (dfaFinalities.count(i) != 0) {
				printf("%2d ^%1d{\t", i, dfaFinalities[i]);	
			} else {
				printf("%2d   {\t", i);	
			}
			
			for (int c = 0; c < CHARSET_SIZE; c++) {
				if (!isUseful[c]) {
					continue;
				}
				printf("{");
				if (dfaEdges[i][c] != ERROR_STATE) {
					printf("%2d ", dfaEdges[i][c]);
				} else {
					printf("   ");
				}
				printf("},\t");
			}
			printf(" }\n");
		}
	}

	void printEdgeTable(EdgeTable edgeTable)
	{
		printf("edge:{\n");
		for (Edge edge : edgeTable) {
			printf("\t%d --'%c'-> %d\n", edge.from, edge.eChar, edge.to);
		}
		printf("}\n");
	}

	void printStr(char *src, int st, int ed)
	{
		for (int i = st; i < ed; i++) {
			printf("%c", src[i]);
		}
	}

	void printTokens(vector<Token> tokens, char *src)
	{
		printf("tokens:{\n");
		for (Token token : tokens) {
			printf("\t<%d> {[%d,%d]: '", token.type, token.st, token.ed - 1);
			printStr(src, token.st, token.ed);
			printf("'}\n");
		}
		printf("}\n");
	}
}
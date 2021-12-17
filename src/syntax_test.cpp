#include "utils.h"
#include "syntax.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
using namespace compiler;

// 获取tokens
vector<Token> getTokensWithDebug(DFA dfa, const string &src) {
	vector<Token> tokens;
	int st = 0;
	int state = 0;

	for (int i = 0; i < src.size(); i++) {
		// printf("\n");
		cout << src << endl;
		for (int j = 0; j < st; j++) {
			printf(" ");
		}
		printf(T_LIGHT_CYAN "^" T_NONE);
		for (int j = st + 1; j < i; j++) {
			printf(" ");
		}
		if (st != i) {
			printf(T_LIGHT_CYAN "^" T_NONE);
		}
		printf("\n");
		// printf("state: s%d, next: '%c'\n", state, src[i]);

		int c = src[i];
		int nextState = dfa.graph[state].count(c) ? 
						dfa.graph[state][c] : 
						ERROR_STATE;

		if (nextState == ERROR_STATE) {
			if (dfa.finality[state] != 0) {
				// 匹配一个token
				int type = dfa.finality[state];
				string val = src.substr(st, i - st);
				tokens.push_back({type, val});
				printf("push token [");
				cout << type << ", '" << val << "']" << endl;
				st = i;
				state = 0;
				i--;
			} else {
				ERR_LOG("syntax error: unexpected char '%c' at column %d\n", c, i);
			}
		} else {
			state = nextState;
		}
	}
	// 尾处理
	if (dfa.finality[state] != 0) {
		// 匹配一个token
		int type = dfa.finality[state];
		string val = src.substr(st, (int)src.size() - st);
		tokens.push_back({type, val});
	} else {
		ERR_LOG("syntax error: unexpected end\n");
	}

	return tokens;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: syntax_test <RE_FILE> <INPUT_FILE> \n");
	}

	ifstream reFile(argv[1], ios::in);
	ifstream inputFile(argv[2], ios::in);

	// initialize re and input
	vector<string> re;
	while (!reFile.eof()) {
		string line;
		getline(reFile, line);
		if (line.length() == 0) {
			continue;
		}
		re.push_back(line);
	}

	string input;
	while (!inputFile.eof()) {
		string line;
		getline(inputFile, line);
		if (line.length() == 0) {
			continue;
		}
		input += line;
		input += '\n';
	}
	rtrim(input);

	// print re and input
	printf("> regular expressions: \n");
	for (int i = 0; i < re.size(); i++) {
		printf("%d: %s\n", i + 1, re[i].c_str());
	}
	printf("> input string: \n");
	printf("'%s'\n", input.c_str());

	// re => dfa
	vector<DFA> dfas(re.size());
	for (int i = 0; i < re.size(); i++) {
		dfas[i] = getMinimizedDfa(getDFAfromNFA(getNFAfromRE(re[i])));
		for (auto it = dfas[i].finality.begin(); 
				it != dfas[i].finality.end(); it++) {
			if (it->second != 0) {
				it->second = i + 1;
			}
		}
		printf("> dfa %d: \n", i + 1);
		printDFA(dfas[i]);
	}
	DFA dfai = getDFAintegrated(dfas);
	printDFA(dfai);

	// dfa => tokens, show step-to-step information
	printf("> syntax analyzing\n");
	vector<Token> tokens = getTokensWithDebug(dfai, input);

	// print tokens
	printf("> tokens: \n");
	for (const Token &token : tokens) {
		printf("[type: %d, val: '%s']\n", token.type, token.val.c_str());
	}

	return 0;
}
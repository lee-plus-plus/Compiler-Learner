#include "utils.h"
#include "lexical.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
using namespace compiler;

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: lexical_test <GRAMMAR_FILE> <INPUT_FILE> \n");
	}

	// initialize grammar and input
	ifstream grammarFile(argv[1], ios::in);
	ifstream inputFile(argv[2], ios::in);

	vector<string> grammarStr;
	while (!grammarFile.eof()) {
		string line;
		getline(grammarFile, line);
		if (line.length() == 0) {
			continue;
		}
		grammarStr.push_back(line);
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

	// print grammar and input
	printf("> productions in grammar: \n");
	for (int i = 0; i < grammarStr.size(); i++) {
		printf("%d: %s\n", i + 1, grammarStr[i].c_str());
	}
	printf("> input string: \n");
	printf("'%s'\n", input.c_str());

	// grmmar => lr(1) analysis table
	Grammar grammar = getGrammarFromStr(grammarStr);
	auto [covers, edgeTable] = getLR1dfa(grammar);
	map<pair<int, int>, Action> analysisTable =  getLR1table(
		grammar.productions, covers, edgeTable
	);
	GrammarNode *root = getLR1grammarTree(
		grammar.productions, analysisTable, input
	);

	printGrammar(grammar);
	printAnalysisTable(analysisTable);
	printGrammarTree(root);

	return 0;
}
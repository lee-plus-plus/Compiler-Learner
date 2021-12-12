#include <iostream>
#include <vector>
#include <map>
using namespace std;

void printGrammarTree(int x, vector<int> isLast = {}) {
	printf("%d", x);
}

int main()
{
	vector<int> x({1, 2, 3});
	printGrammarTree(1);
	return 0;
}
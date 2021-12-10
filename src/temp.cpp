#include <iostream>
#include <fstream>
#include <map>
using namespace std;

int main()
{
	map<int, int> x({{1, 3}, {2, 3}, {3, 4}});
	for (auto it = x.begin(); it != x.end(); it++) {
		printf("%d", it - x.begin());
	}

	return 0;
}
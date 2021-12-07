#include <iostream>
#include <map>
#include <set>
using namespace std;

int main()
{
	map<int, set<int>> s;
	s[0] = ({0});

	for (int i : s[0]) {
		cout << i << " ";
	}

	return 0;
}
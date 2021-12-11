#include <iostream>
#include <string>
#include <map>
using namespace std;

int main()
{
	pair<int, string> x = {4, "fuck"};
	auto [a, b] = x;

	printf("%d, %s", a, b.c_str());

	return 0;
}
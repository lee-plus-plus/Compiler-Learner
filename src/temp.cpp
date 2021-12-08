#include <iostream>
#include <map>
#include <set>
using namespace std;


int main()
{
	map<int, int> a;
	a[3] = 5;

	// int x = a[1];
	printf("%d\n", a.count(5));


	return 0;
}
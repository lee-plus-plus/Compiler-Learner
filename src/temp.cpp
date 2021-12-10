#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <functional>
using namespace std;

struct Prod
{
	int val;
	bool operator<(const Prod &p) const {
		return less<int>()(val, p.val);
	}
};

struct Prod2 : Prod {
	int val2;
	Prod2(int val, int val2): 
		Prod({val}), val2(val2) {};
	bool operator<(const Prod2 &p) const {
		return less<pair<int, int>>()({val, val2}, {p.val, p.val2});
	}
};

const char *f(char c)
{
	string src(1, c);
	return src.c_str();
}

struct MyMap : map<int, map<int, int>> {
	map<int, int> search(int i) {
		if (this->count(i) == 0) {
			return map<int, int>({});
		} else {
			return this->at(i);
		}
	}
};

int ff(int x, int y)
{
	return x + y;
}

// struct DFAnode : pair<const int, map<int, int>> {
// 	const int &from = this->first;
// 	DFAedge &edge = this->second;
// 	int &symbol = edge.symbol;
// 	int &to = edge.to;
// };

typedef map<int, map<int, int>> DFAgraph;

int main()
{
	vector<int> x({4});
	printf("%d", x[0]);
	return 0;
}
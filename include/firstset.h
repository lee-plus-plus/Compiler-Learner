#ifndef SETGRAPH_H
#define SETGRAPH_H
#include "defs.h"
#include "nfa2dfa.h"
using namespace std;

namespace lexical {

	// 求首符集
	set<char> getFirstSet(char *src);
	// 求随符集
	set<char> getFollowSet(char *src);

}



# endif
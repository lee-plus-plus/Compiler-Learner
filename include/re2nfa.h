#ifndef RE2NFA_H
#define RE2NFA_H
#include "defs.h"
using namespace std;

namespace compiler {
	// RE转NFA邻接表
	EdgeTable re2nfa(const string &re, int length, int &numStates);

	void splitReByMid(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);
	void splitReByParen(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);

	void testRe2Nfa();
}

#endif
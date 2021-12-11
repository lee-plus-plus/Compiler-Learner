#ifndef SYNTAX_H
#define SYNTAX_H
#include "defs.h"
using namespace std;

namespace compiler {

	// 消除DFA不可达状态, 并整理状态下标
	// 返回DFA和各状态的可终结性
	DFA getReachableDfa(DFA dfa);

	// 合并DFA等价状态
	// 可终结性相同且跳转等效的节点将会被合并
	// 用不同值标注DFA的节点可终结属性, 可以防止合并
	// 返回DFA和各状态的可终结性
	DFA getMergedDfa(DFA dfa);

	// 最小化DFA
	DFA getMinimizedDfa(DFA dfa);

	// 扩张覆盖片选择（epsilon-闭包法）
	void setCoverExpanded(set<int> &cover, NFAgraph nfa);
	// 求后继覆盖片
	map<int, set<int>> getNextCovers(set<int> cover, NFAgraph nfa);

	// 将NFA转为DFA
	// 得到的DFA每个状态都对应NFA的一个覆盖片，得到的DFA并非最小
	// 由于覆盖片中各节点可能拥有不同可终结属性, 覆盖片(DFA节点)的可终结属性无法确定
	// 返回DFA和覆盖片，其中覆盖片记录了DFA-NFA节点映射关系, 不负责处理转换后的可终结属性
	pair<DFAgraph, map<int, set<int>>> getCoverMapfromNFAgraph(
		NFAgraph nfaGraph);

	// 默认确定finality方式: 取最大值
	map<int, int> getFinalityFromCoverMap(map<int, int> nfaFinality, 
										  map<int, set<int>> coverMap);

	// 将NFA转为DFA
	// 采用默认方式确定覆盖片(DFA节点)的可终结属性
	// 返回DFA和各状态的可终结性
	DFA getDFAfromNFA(NFA nfa);
	
	// RE转NFA
	// 返回NFA和各状态的可终结性, 默认可终结为1, 其余为0
	NFA getNFAfromRE(const string &re);

	// 将若干个DFA整合为1个大DFA
	// 每个DFA均以0为起始状态，合并得到唯一起始状态
	// 用不同值标注DFA的节点可终结属性, 可以防止合并
	// 得到的DFA并非最小
	DFA getDFAintegrated(vector<DFA> dfas);

	// 输入字符串和dfa, 获取tokens
	// 为简化问题，token中存放截取的字符串, 可供进一步处理
	vector<Token> getTokens(DFA dfa, const string &src);
}



#endif

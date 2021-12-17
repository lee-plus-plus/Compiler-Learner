#include "syntax.h"
#include "utils.h"
#include <algorithm>
using namespace std;

namespace compiler {

void splitReByMid(const string &re, int st, int ed, int &numStates, 
				  EdgeTable &edges, int stState, int edState);
void splitReByParen(const string &re, int st, int ed, int &numStates, 
					EdgeTable &edges, int stState, int edState);

// NFAgraph转DFAgraph, 并返回覆盖片(DFAgraph结点)的构造
pair<DFAgraph, map<int, set<int>>> getCoverMapfromNFAgraph(
	NFAgraph nfaGraph)
{
	vector<set<int>> covers;	// dfa节点对原nfa节点的映射
	DFAgraph dfaGraph;

	// 构造初始覆盖片
	set<int> initCover({0});
	setCoverExpanded(initCover, nfaGraph);
	covers.push_back(initCover);

	// bfs, 产生新覆盖片
	for (int coverIdx = 0; (size_t)coverIdx < covers.size(); coverIdx++) {
		set<int> cover = covers[coverIdx];

		set<int> nextSymbols;
		for (int state : cover) {
			for (const auto &node : nfaGraph[state]) {
				nextSymbols.insert(node.first);
			}
		}
		// 对覆盖片步进，产生新覆盖片
		map<int, set<int>> nextCovers = getNextCovers(cover, nfaGraph);
		for (const auto &elem : nextCovers) {
			int symbol = elem.first;
			set<int> nextCover = elem.second;

			if (symbol == EMPTY_CHAR) {
				continue;
			}
			// 为覆盖片分配下一个idx，加入
			auto it = find(covers.begin(), covers.end(), nextCover);
			int nextCoverIdx = (int)(it - covers.begin()); 
			if ((size_t)nextCoverIdx == covers.size()) {
				covers.push_back(nextCover); // 无重复, 先生成新覆盖片
			}
			dfaGraph[coverIdx][symbol] = nextCoverIdx;
		}
	}

	// 格式转换, covers -> coverMap
	map<int, set<int>> coverMap;
	for (int coverIdx = 0; (size_t)coverIdx < covers.size(); coverIdx++) {
		for (int nfaState : covers[coverIdx]) {
			coverMap[coverIdx].insert(nfaState);
		}
	}

	return make_pair(dfaGraph, coverMap);
}

DFA getDFAfromNFA(NFA nfa)
{
	auto [dfaGraph, coverMap] = getCoverMapfromNFAgraph(nfa.graph);
	auto finality = getFinalityFromCoverMap(nfa.finality, coverMap);
	return DFA({dfaGraph, finality});
}

map<int, int> getFinalityFromCoverMap(map<int, int> nfaFinality, 
									  map<int, set<int>> coverMap)
{
	map<int, int> finality;
	for (const auto &elem : coverMap) {
		for (int i : elem.second) {
			finality[elem.first] = max(
				finality[elem.first], 
				nfaFinality[i]
			);
		}
	}
	return finality;
}

// 最小化DFA
DFA getMinimizedDfa(DFA dfa)
{
	return getReachableDfa(getMergedDfa(dfa));
}

// 消除DFA不可达状态, 并整理DFA状态数字至0 ~ numStates-1
DFA getReachableDfa(DFA dfa)
{
	DFA resDfa;
	vector<int> idState({0});
	map<int, int> stateId({{0, 0}});
	// bfs
	for (int id = 0; id < idState.size(); id++) {
		for (const auto &elem : dfa.graph[idState[id]]) {
			int symbol = elem.first;
			int nextState = elem.second;
			// assign new id
			if (stateId.count(nextState) == 0) {
				stateId[nextState] = idState.size();
				idState.push_back(nextState);
			}
			// replace
			resDfa.graph[id][symbol] = stateId[nextState];
		}
	}
	for (const auto &elem : stateId) {
		resDfa.finality[elem.second] = dfa.finality[elem.first];
	}
	
	return resDfa;
}


// 合并DFA等价状态
DFA getMergedDfa(DFA dfa)
{
	// 用不同的整数(id)染色, 区分集合划分
	// 按可终结属性值的不同进行初次划分
	map<int, int> stateColor = dfa.finality;
	// 获取充分染色后DFA节点, 作为区分特征
	// 对于状态i和状态j,若{stateColor[i], dfa}
	typedef pair<int, map<int, int>> DFAnode;
	auto getColoredDFAnode = [&stateColor](DFAnode node) -> DFAnode { 
		node.first = stateColor[node.first];
		for (auto it = node.second.begin(); it != node.second.end(); it++) {
			it->second = stateColor[it->second];
		}
		return node;
	};

	int numSplitedStates = 1;
	map<DFAnode, set<int>> partition;
	while (true) {
		// 按后继状态的染色情况是否相同, 进行进一步划分
		partition = {};
		for (auto [state, _] : dfa.finality) {
			auto coloredNode = getColoredDFAnode({state, dfa.graph[state]});
			partition[coloredNode].insert(state);
		}
		// 仅当无更新才结束
		if (numSplitedStates == partition.size()) {
			break;
		}
		numSplitedStates = partition.size();
		// 根据划分重新染色
		int newStateColor = 0;
		for (auto it = partition.begin(); it != partition.end(); it++) {
			for (int state : it->second) {
				stateColor[state] = newStateColor;
			}
			newStateColor++;
		}
	}
	// 根据染色, 构建状态替换映射
	map<int, int> idState;
	for (const auto &elem : stateColor) {
		if (idState.count(elem.second) == 0) {
			idState[elem.second] = elem.first;
		} else {
			idState[elem.second] = min(idState[elem.second], elem.first);
		}
	}
	map<int, int> replaceMap;	
	for (const auto &elem : stateColor) {
		replaceMap[elem.first] = idState[stateColor[elem.first]];
	}
	// 替换
	EdgeTable edgeTable = toEdgeTable(dfa.graph);
	for (auto it = edgeTable.begin(); it != edgeTable.end(); it++) {
		it->from = replaceMap[it->from];
		it->to = replaceMap[it->to];
	}
	DFA resDfa;
	resDfa.graph = toDFAgraph(edgeTable);
	map<int, int> resFinality;
	for (auto it = dfa.finality.begin(); it != dfa.finality.end(); it++) {
		resDfa.finality[replaceMap[it->first]] = it->second;
		// printf("{%d, %d} ", replaceMap[it->first], it->second);
	}

	return resDfa;
}

// 扩张覆盖片选择（epsilon-闭包法）
void setCoverExpanded(set<int> &cover, NFAgraph nfaGraph)
{	
	// bfs扩大搜索
	queue<int> q;
	for (int state : cover) {
		q.push(state);
	}
	while (q.size()) {
		int current = q.front();
		q.pop();
		for (const auto &elem : nfaGraph[current]) {
			int symbol = elem.first;
			int next = elem.second;
			if (symbol == EMPTY_CHAR && cover.count(next) == 0) {
				cover.insert(next);
				q.push(next);
			}
		}
	}
}

// 求后继覆盖片
map<int, set<int>> getNextCovers(set<int> cover, NFAgraph nfaGraph)
{
	map<int, set<int>> nextCovers;
	for (int current : cover) {
		for (const auto &edge : nfaGraph[current]) {
			if (edge.first != EMPTY_CHAR) {
				nextCovers[edge.first].insert(edge.second);
			}
		}
	}
	for (auto it = nextCovers.begin(); it != nextCovers.end(); it++) {
		setCoverExpanded(it->second, nfaGraph);
	}
	return nextCovers;
}

// RE转NFA邻接表
NFA getNFAfromRE(const string &re)
{
	int numStates = 2;
	EdgeTable edgeTable;
	splitReByMid(
		re, 0, re.size(),  
		numStates, edgeTable, 0, 1
	);

	map<int, int> finality;
	for (int i = 0; i < numStates - 1; i++) {
		finality[i] = 0;	
	}
	finality[1] = 1;

	return NFA({toNFAgraph(edgeTable), finality});
}

// RE转NFA递归函数: 提取'|'并联
void splitReByMid(const string &re, int st, int ed, int &numStates, 
				  EdgeTable &edges, int stState, int edState)
{
	// fprintf(stderr, "bymid: \t%s\n", re.substr(st, ed - st).c_str());
	int parenLevel = 0;
	int midPos = st - 1;
	for (int i = st; i < ed; i++) {
		switch (re[i]) {
			case '|':
				if (parenLevel != 0) {
					break;
				}
				splitReByParen(
					re, midPos + 1, i, 
					numStates, edges, stState, edState
				);
				midPos = i;
				break;
			case '(':
				parenLevel++;
				break;
			case ')':
				parenLevel--;
				if (parenLevel < 0) {
					ERR_LOG("syntax error: mismatched parenthesis ')'");
				}
				break;
			case '\\':
				if (i + 1 >= ed) {
					ERR_LOG("syntax error: separate escape character '\\'");
				}
				i++;
				// no break
			default:
				break;
		}
	}
	if (parenLevel != 0) {
		// ERR_LOG("%d", parenLevel);
		ERR_LOG("syntax error: unmatched parenthesis '('");
	}
	splitReByParen(
		re, midPos + 1, ed, 
		numStates, edges, stState, edState
	);
}

// RE转NFA递归函数: 提取'('')'串联
void splitReByParen(const string &re, int st, int ed, int &numStates, 
					EdgeTable &edges, int stState, int edState)
{
	// fprintf(stderr, "paren: \t%s\n", re.substr(st, ed - st).c_str());
	int parenLevel = 0;
	int lparenPos = -1;
	int lparenState = stState;
	int edState2 = numStates++;
	for (int i = st; i < ed; i++) {
		switch (re[i]) {
			case '(':
				if (parenLevel == 0) {
					lparenPos = i;
					lparenState = stState;
				}
				parenLevel++;
				break;
			case ')': 
				parenLevel--; 
				if (parenLevel < 0) {
					ERR_LOG("syntax error: mismatched parenthesis ')'");
				}
				
				if (parenLevel == 0) {
					if (i + 1 < ed && re[i + 1] == '*') {

						splitReByMid(
							re, lparenPos + 1, i, 
							numStates, edges, stState, stState
						);
						i++;
					} else if (i + 1 < ed && re[i + 1] == '+') {
						splitReByMid(
							re, lparenPos + 1, i, 
							numStates, edges, stState, stState
						);
						splitReByMid(
							re, lparenPos + 1, i, 
							numStates, edges, stState, edState2
						);
						stState = edState2;
						edState2 = numStates++;
						i++;
					} else {
						splitReByMid(
							re, lparenPos + 1, i, 
							numStates, edges, stState, edState2
						);
						stState = edState2;
						edState2 = numStates++;						
					}
				}
				break;
			case '|': 
				assert(parenLevel > 0);
				break;
			case '*': 
				if (parenLevel == 0) {
					ERR_LOG("syntax error: mismatched '*'");
				}
				break;
			case '+':
				if (parenLevel == 0) {
					ERR_LOG("syntax error: mismatched '+'");
				}
				break;
			case '\\':
				if (parenLevel == 0) {
					if (i + 1 >= ed) {
						ERR_LOG("syntax error: separate escape character '\\'");
					}
					i++;
				}
				// no break
			default:
				if (parenLevel == 0) {
					if (i + 1 < ed && re[i + 1] == '*') {
						edges.push_back({re[i], stState, stState});
						i++;
					} else if (i + 1 < ed && re[i + 1] == '+') {
						edges.push_back({re[i], stState, stState});
						edges.push_back({re[i], stState, edState2});
						stState = edState2;
						edState2 = numStates++;
						i++;
					} else {
						edges.push_back({re[i], stState, edState2});
						stState = edState2;
						edState2 = numStates++;						
					}
				}
				break;
			
		}
	}
	if (parenLevel != 0) {
		ERR_LOG("syntax error: unmatched parenthesis '('");
	}
	edges.push_back({EMPTY_CHAR, stState, edState});
}

// 将若干个DFA整合为1个大DFA
DFA getDFAintegrated(vector<DFA> dfas)
{
	vector<EdgeTable> edgeTables(dfas.size());
	for (int i = 0; i < dfas.size(); i++) {
		dfas[i] = getMinimizedDfa(dfas[i]);
		edgeTables[i] = toEdgeTable(dfas[i].graph);
	}

	EdgeTable nfaEdgeTable;
	map<int, int> nfaFinality;

	// 更新规则很简单，就是把每个状态的重编号，把边集求合
	// states = {0, dfa0.states, dfa1.states, ...}
	int numStateAdded = 1;
	for (int i = 0; i < dfas.size(); i++) {
		// 更新边集合
		nfaEdgeTable.push_back({EMPTY_CHAR, 0, numStateAdded});
		for (Edge &edge : edgeTables[i]) {
			nfaEdgeTable.push_back({
				edge.symbol, 
				edge.from + numStateAdded, 
				edge.to + numStateAdded
			});
		}
		// 更新终结状态集合
		for (int state = 0; state < dfas[i].finality.size(); state++) {
			nfaFinality[state + numStateAdded] = dfas[i].finality[state];
		}

		numStateAdded += dfas[i].finality.size();
	}

	NFA nfa = {toNFAgraph(nfaEdgeTable), nfaFinality};
	DFA dfa = getDFAfromNFA(nfa);
	dfa = getMinimizedDfa(dfa);
	return dfa;
}

// 获取tokens
vector<Token> getTokens(DFA dfa, const string &src)
{
	vector<Token> tokens;
	int st = 0;
	int state = 0;

	for (int i = 0; i < src.size(); i++) {
		// printf("i=%d: s%d, next=%c\n", i, state, src[i]);
		int c = src[i];
		int nextState = dfa.graph[state].count(c) ? 
						dfa.graph[state][c] : 
						ERROR_STATE;

		if (nextState == ERROR_STATE) {
			if (dfa.finality[state] != 0) {
				// 匹配一个token
				int type = dfa.finality[state];
				string val = src.substr(st, i - st);
				tokens.push_back({type, val});
				// printf("push token[%d, %s]\n", i, type, val.c_str());
				st = i;
				state = 0;
				i--;
			} else {
				ERR_LOG("syntax error: unexpected char '%c'\n", c);
			}
		} else {
			state = nextState;
		}
	}
	// 尾处理
	if (dfa.finality[state] != 0) {
		// 匹配一个token
		int type = dfa.finality[state];
		string val = src.substr(st, (int)src.size() - st);
		tokens.push_back({type, val});
	} else {
		ERR_LOG("syntax error: unexpected end\n");
	}

	return tokens;
}

} // namespace compiler

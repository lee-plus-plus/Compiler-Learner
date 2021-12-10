#include "syntax.h"
#include "utils.h"
#include <algorithm>
using namespace std;

namespace compiler {

	void splitReByMid(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);
	void splitReByParen(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState);

	// NFA转DFA（未最小化）
	// return: dfa, dfaNode-nfaNodes映射关系
	pair<DFAgraph, map<int, set<int>>> getNfa2DfaCoverMap(NFAgraph nfa, map<int, int> finality)
	{
		vector<set<int>> covers;	// dfa节点对原nfa节点的映射
		DFAgraph dfa;

		// 构造初始覆盖片
		set<int> initCover({0});
		setCoverExpanded(initCover, nfa);
		covers.push_back(initCover);

		// bfs, 产生新覆盖片
		for (int coverIdx = 0; (size_t)coverIdx < covers.size(); coverIdx++) {
			set<int> cover = covers[coverIdx];

			set<int> nextSymbols;
			for (int state : cover) {
				for (const auto &node : nfa[state]) {
					nextSymbols.insert(node.first);
				}
			}
			// 对覆盖片步进，产生新覆盖片
			map<int, set<int>> nextCovers = getNextCovers(cover, nfa);
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
				dfa[coverIdx][symbol] = nextCoverIdx;
			}
		}

		// 格式转换, covers -> coverMap
		map<int, set<int>> coverMap;
		for (int coverIdx = 0; (size_t)coverIdx < covers.size(); coverIdx++) {
			for (int nfaState : covers[coverIdx]) {
				coverMap[coverIdx].insert(nfaState);
			}
		}

		return make_pair(dfa, coverMap);
	}

	pair<DFAgraph, map<int, int>> getNfa2Dfa(NFAgraph nfa, map<int, int> finality)
	{
		auto result = getNfa2DfaCoverMap(nfa, finality);
		map<int, int> resFinality = getFinalityFromCoverMap(finality, result.second);
		return make_pair(result.first, resFinality);
	}

	map<int, int> getFinalityFromCoverMap(map<int, int> nfaFinality, map<int, set<int>> coverMap)
	{
		map<int, int> finality;
		for (const auto &elem : coverMap) {
			for (int i : elem.second) {
				finality[elem.first] = max(finality[elem.first], nfaFinality[i]);
			}
		}
		return finality;
	}
	
	// 最小化DFA
	pair<DFAgraph, map<int, int>> getMinimizedDfa(DFAgraph dfa, map<int, int> finality)
	{
		auto result1 = make_pair(dfa, finality);
		result1 = getMergedDfa(result1.first, result1.second);
		result1 = getReachableDfa(result1.first, result1.second);
		return result1;
	}
	
	// 消除DFA不可达状态, 并整理DFA状态数字至0 ~ numStates-1
	pair<DFAgraph, map<int, int>> getReachableDfa(DFAgraph dfa, map<int, int> finality)
	{
		DFAgraph resDfa;
		map<int, int> resFinality;
		vector<int> idState({0});
		map<int, int> stateId({{0, 0}});

		// bfs
		for (int id = 0; id < idState.size(); id++) {
			for (const auto &elem : dfa[idState[id]]) {
				int symbol = elem.first;
				int nextState = elem.second;
				// assign new id
				if (stateId.count(nextState) == 0) {
					stateId[nextState] = idState.size();
					idState.push_back(nextState);
				}
				// replace
				resDfa[id][symbol] = stateId[nextState];
			}
		}
		for (const auto &elem : stateId) {
			resFinality[elem.second] = finality[elem.first];
		}
		
		return make_pair(resDfa, resFinality);
	}
	
	
	// 合并DFA等价状态
	pair<DFAgraph, map<int, int>> getMergedDfa(DFAgraph dfa, map<int, int> finality)
	{
		// dfa状态有差异条件: 1. 可终结属性不同, 2. 后继集合所属划分不同; 反复迭代, 直至无法找出差异
		// 用不同的整数(id)染色,区分集合划分
		// 按可终结属性值的不同进行初次划分
		map<int, int> stateId = finality;
		// 获取充分染色后DFA节点, 作为区分特征
		typedef pair<int, DFAnode> Node;
		typedef pair<int, DFAnode> ColoredNode;
		auto getNodeColored = [&stateId](Node node) -> ColoredNode { 
			node.first = stateId[node.first];
			for (auto it = node.second.begin(); it != node.second.end(); it++) {
				it->second = stateId[it->second];
			}
			return node;
		};

		int numSplitedStates = 1;
		map<ColoredNode, set<int>> splitedStates;
		while (true) {
			// 按后继状态的染色情况是否相同, 进行进一步划分
			splitedStates = {};
			for (const auto &elem : finality) {
				int state = elem.first;
				splitedStates[getNodeColored({state, dfa[state]})].insert(state);
			}
			// 仅当无更新才结束
			if (numSplitedStates == splitedStates.size()) {
				break;
			}
			numSplitedStates = splitedStates.size();
			// 根据划分重新染色
			int colorId = 0;
			for (auto it = splitedStates.begin(); it != splitedStates.end(); it++) {
				for (int state : it->second) {
					stateId[state] = colorId;
				}
				colorId++;
			}
		}
		// 根据染色, 构建状态替换映射
		map<int, int> idState;
		for (const auto &elem : stateId) {
			if (idState.count(elem.second) == 0) {
				idState[elem.second] = elem.first;
			} else {
				idState[elem.second] = min(idState[elem.second], elem.first);
			}
		}
		map<int, int> replaceMap;	
		for (const auto &elem : stateId) {
			replaceMap[elem.first] = idState[stateId[elem.first]];
		}
		// 替换
		EdgeTable edgeTable = toEdgeTable(dfa);
		for (auto it = edgeTable.begin(); it != edgeTable.end(); it++) {
			it->from = replaceMap[it->from];
			it->to = replaceMap[it->to];
		}
		DFAgraph resDfa = toDFAgraph(edgeTable);
		map<int, int> resFinality;
		for (auto it = finality.begin(); it != finality.end(); it++) {
			resFinality[replaceMap[it->first]] = it->second;
			// printf("{%d, %d} ", replaceMap[it->first], it->second);
		}

		return make_pair(resDfa, resFinality);
	}
	
	// 扩张覆盖片选择（epsilon-闭包法）
	void setCoverExpanded(set<int> &cover, NFAgraph nfa)
	{	
		// bfs扩大搜索
		queue<int> q;
		for (int state : cover) {
			q.push(state);
		}
		while (q.size()) {
			int current = q.front();
			q.pop();
			for (const auto &edge : nfa[current]) {
				if (edge.first == EMPTY_CHAR && cover.count(edge.second) == 0) {
					cover.insert(edge.second);
					q.push(edge.second);
				}
			}
		}
	}

	// 求后继覆盖片
	map<int, set<int>> getNextCovers(set<int> cover, NFAgraph nfa)
	{
		map<int, set<int>> nextCovers;
		for (int current : cover) {
			for (const auto &edge : nfa[current]) {
				if (edge.first != EMPTY_CHAR) {
					nextCovers[edge.first].insert(edge.second);
				}
			}
		}
		for (auto it = nextCovers.begin(); it != nextCovers.end(); it++) {
			setCoverExpanded(it->second, nfa);
		}
		return nextCovers;
	}
	
	// RE转NFA邻接表
	pair<NFAgraph, map<int, int>> re2nfa(const string &re)
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

		return make_pair(toNFAgraph(edgeTable), finality);
	}
	
	// RE转NFA递归函数: 提取'|'并联
	void splitReByMid(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState)
	{
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
			ERR_LOG("syntax error: unmatched parenthesis '('");
		}
		splitReByParen(
			re, midPos + 1, ed, 
			numStates, edges, stState, edState
		);
	}

	// RE转NFA递归函数: 提取'('')'串联
	void splitReByParen(const string &re, int st, int ed, int &numStates, EdgeTable &edges, int stState, int edState)
	{
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
					ERR_LOG("syntax error: mismatched '*'");
					break;
				case '+':
					ERR_LOG("syntax error: mismatched '+'");
					break;
				case '\\':
					if (i + 1 >= ed) {
						ERR_LOG("syntax error: separate escape character '\\'");
					}
					i++;
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
	pair<DFAgraph, map<int, int>> getDFAintegrated(vector<DFAgraph> dfas, vector<map<int, int>> dfaFinalities)
	{
		vector<EdgeTable> edgeTables;
		for (int i = 0; i < dfas.size(); i++) {
			auto result = getMinimizedDfa(dfas[i], dfaFinalities[i]);
			edgeTables.push_back(toEdgeTable(result.first));
			dfaFinalities[i] = result.second;
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
			for (int state = 0; state < dfaFinalities[i].size(); state++) {
				nfaFinality[state + numStateAdded] = dfaFinalities[i][state];
			}

			numStateAdded += dfaFinalities[i].size();
		}

		NFAgraph nfa = toNFAgraph(nfaEdgeTable);
		auto result = getNfa2Dfa(nfa, nfaFinality);

		return result;
	}
	
	// 获取tokens
	vector<Token> getTokens(DFAgraph dfa, map<int, int> finality, const string &src)
	{
		vector<Token> tokens;
		int st = 0;
		int state = 0;

		for (int i = 0; i < src.size(); i++) {
			int c = src[i];
			int nextState = dfa[state][c];

			if (nextState == ERROR_STATE) {
				if (finality[state] != 0) {
					// 匹配一个token
					int type = finality[state];
					string val = src.substr(st, i - st);
					tokens.push_back({type, val});
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
		if (finality[state] != 0) {
			// 匹配一个token
			int type = finality[state];
			string val = src.substr(st, (int)src.size() - st);
			tokens.push_back({type, val});
		} else {
			ERR_LOG("syntax error: unexpected end\n");
		}

		return tokens;
	}

}

#include "syntax.h"
#include "utils.h"
#include <algorithm>
using namespace std;

namespace compiler {
	
	// NFA转DFA（未最小化）
	// return: dfa, dfaNode-nfaNodes映射关系
	pair<DFAgraph, multimap<int, int>> nfa2dfa(NFAgraph nfa, map<int, int> finality)
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
		multimap<int, int> coverMap;
		for (int coverIdx = 0; (size_t)coverIdx < covers.size(); coverIdx++) {
			for (int nfaState : covers[coverIdx]) {
				coverMap.insert({coverIdx, nfaState});
			}
		}

		return make_pair(dfa, coverMap);
	}
	
	// 最小化DFA
	pair<DFAgraph, map<int, int>> getMinimizedDfa(DFAgraph dfa, map<int, int> finality)
	{
		auto result1 = make_pair(dfa, finality);
		result1 = getEmptyStatesEliminatedDfa(result1.first, result1.second);
		result1 = getEqualStatesCombinedDfa(result1.first, result1.second);
		return result1;
		// return getEmptyStatesEliminatedDfa(result1.first, result1.second);
	}
	
	// 消除DFA不可达状态, 并整理DFA状态数字至0 ~ numStates-1
	pair<DFAgraph, map<int, int>> getEmptyStatesEliminatedDfa(DFAgraph dfa, map<int, int> finality)
	{
		DFAgraph resDfa;
		map<int, int> resFinality;
		vector<int> idState({0});
		map<int, int> stateId({{0, 0}});

		// bfs
		for (int id = 0; id < idState.size(); id++) {
			for (const auto &elem : dfa[id]) {
				int symbol = elem.first;
				int nextState = elem.second;
				// assign new id
				if (stateId.count(nextState) == 0) {
					stateId[nextState] = idState.size();
					idState.push_back(nextState);
				}
				// replace
				resDfa[id][symbol] = stateId[nextState];
				resFinality[id] = finality[idState[id]];
			}
		}
		
		return make_pair(resDfa, resFinality);
	}
	
	
	// 合并DFA等价状态
	pair<DFAgraph, map<int, int>> getEqualStatesCombinedDfa(DFAgraph dfa, map<int, int> finality)
	{
		// dfa状态有差异条件: 1. 可终结属性不同, 2. 后继集合所属划分不同; 反复迭代, 直至无法找出差异
		// 用不同的整数(id)染色,区分集合划分
		// 按可终结属性值的不同进行初次划分
		map<int, int> stateId = finality;
		map<int, set<int>> idStates;
		for (const auto &elem : stateId) {
			idStates[stateId[elem.first]].insert(elem.second);
		}
		int nextId;
		for (nextId = 0; idStates.count(nextId) != 0; nextId++) {}

		// 获取该状态的后继集染色情况, 用于比较两状态后继集染色差异
		auto getNodeWithId = [&stateId](DFAnode &node) -> DFAnode { 
			for (auto it = node.begin(); it != node.end(); it++) {
				it->second = stateId[it->second];
			}
			return node;
		};

		// 按后继状态是否属于同一集合进行进一步划分
		while (true) {
			bool isUpdated = false; // 是否有更新过

			for (const auto &elem : idStates) {
				int id = elem.first;
				// 生成后继集染色情况
				map<DFAnode, set<int>> splitedStates;
				for (int state : idStates[id]) {
					DFAnode nodeWithId = getNodeWithId(dfa[state]);
					splitedStates[nodeWithId].insert(state);
				}
				// 后继集染色情况有差异
				if (splitedStates.size() != 1) {
					// 对后继集染色情况不同的集合进行差异染色
					for (const auto &stateSet : splitedStates) {
						for (int s : stateSet.second) {
							idStates[stateId[s]].erase(s);
							stateId[s] = nextId;
							idStates[stateId[s]].insert(s);
						}
						idStates.erase(id);
						for (nextId; idStates.count(nextId) != 0; nextId++) {}
					}
					isUpdated = true;
				}
			}
			// 只要有一次更新都必须从头再试一遍
			if (!isUpdated) {
				break;
			}
		}
		// 用等价集合编号代替原先元素
		EdgeTable edgeTable = toEdgeTable(dfa);
		for (auto it = edgeTable.begin(); it != edgeTable.end(); it++) {
			int from2 = *idStates[stateId[it->from]].begin();	
			int to2 = *idStates[stateId[it->to]].begin(); 
			it->from = from2;
			it->to = to2;
		}

		DFAgraph resDfa = toDFAgraph(edgeTable);
		map<int, int> resFinality;
		for (const auto &elem : finality) {
			int state2 = *idStates[stateId[elem.first]].begin();	
			resFinality[state2] = elem.second;
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
				if (edge.first == EMPTY_CHAR) {
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
	NFAgraph re2nfa(const string &re, int &numStates)
	{
		numStates = 2;
		EdgeTable edgeTable;
		splitReByMid(
			re, 0, re.size(),  
			numStates, edgeTable, 0, 1
		);

		return toNFAgraph(edgeTable);
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
			auto result = getEmptyStatesEliminatedDfa(dfas[i], dfaFinalities[i]);
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
		auto result = nfa2dfa(nfa, nfaFinality);
		DFAgraph resDfa = result.first;
		multimap<int, int> resCoverMap = result.second;
		// 假设所得dfa的可结束状态无冲突
		map<int, int> resDfaFinality;
		for (const auto &elem : resCoverMap) {
			resDfaFinality[elem.first] = nfaFinality[elem.second];
		}

		return make_pair(resDfa, resDfaFinality);
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

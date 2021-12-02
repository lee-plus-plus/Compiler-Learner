#include <cstdio>
#include <string>
#include <algorithm>
#include <iostream>
#include "nfa2dfa.h"
using namespace std;

namespace compiler {

	// 创建NFA
	NFAedges getNewNFAedges(int numStates, EdgeTable edgeTable)
	{
		NFAedges nfaEdges(numStates);

		for (Edge edge : edgeTable) {
			nfaEdges[edge.from][edge.eChar].push_back(edge.to);
		}

		return nfaEdges;
	}

	// 创建DFA
	DFAedges getNewDFAedges(int numStates, EdgeTable edgeTable)
	{
		DFAedges dfaEdges(numStates);
		for (DFAitem &item : dfaEdges) {
			for (int &i : item) {
				i = ERROR_STATE;
			}
		}

		for (Edge edge : edgeTable) {
			dfaEdges[edge.from][edge.eChar] = edge.to;
		}

		return dfaEdges;
	}

	// 一种简略的创建DFA的方法
	DFAedges getNewDFAedges(DFAedges slicedDFAwithHead)
	{
		DFAedges dfaEdges2(slicedDFAwithHead.size() - 1);
		for (DFAitem &item : dfaEdges2) {
			for (int &i : item) {
				i = ERROR_STATE;
			}
		}
		for (int i = 1; (uint32_t)i < slicedDFAwithHead.size(); i++) {
			for (int j = 0; slicedDFAwithHead[0][j] != 0; j++) {
				dfaEdges2[i - 1][slicedDFAwithHead[0][j]] = slicedDFAwithHead[i][j];
			}
		}

		return dfaEdges2;
	}

	EdgeTable dfaEdges2EdgeTable(DFAedges dfa)
	{
		EdgeTable edgeTable;

		for (int i = 0; i < dfa.size(); i++) {
			for (int c = 0; c < CHARSET_SIZE; c++) {
				if (c == EMPTY_CHAR) {
					continue;
				}
				if (dfa[i][c] != ERROR_STATE) {
					edgeTable.push_back({c, i, dfa[i][c]});
				}
			}
		}

		return edgeTable;
	}

	// NFA转DFA（未最小化）
	pair<DFAedges, set<int>> nfa2dfa(NFAedges nfa, set<int> nfaFinality)
	{
		vector<set<int>> covers;
		DFAedges coversEdges;
		set<int> coversFinality;

		// 默认空行: 全-1
		array<int, CHARSET_SIZE> emptyCoversEdgesRow;
		for (int &elem : emptyCoversEdgesRow) {
			elem = ERROR_STATE;
		}

		// 构造初始覆盖片
		set<int> initCover;
		initCover.insert(0);
		setCoverExpanded(initCover, nfa);

		covers.push_back(initCover);
		coversEdges.push_back(emptyCoversEdgesRow);

		for (int i = 0; (uint32_t)i < covers.size(); i++) {
			set<int> cover = covers[i];
			// printCovers({cover});

			// 标记是否为终结状态
			for (int stateIdx : cover) {
				if (nfaFinality.count(stateIdx) != 0) {
					coversFinality.insert(i);
				}
			}

			for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
				if (charIdx == EMPTY_CHAR) {
					continue;
				}
				set<int> nextCover = getNextCover(cover, nfa, charIdx);

				if (nextCover.size() != 0) {
					auto it = find(covers.begin(), covers.end(), nextCover);
					int tgtCoverIdx = it - covers.begin();

					// 生成新覆盖片
					if ((uint32_t)tgtCoverIdx == covers.size()) {
						covers.push_back(nextCover);
						coversEdges.push_back(emptyCoversEdgesRow);
					}
					// 覆盖片边连接
					coversEdges[i][charIdx] = tgtCoverIdx;

				} else {
					// pass
				}
			}
		}

		return pair<DFAedges, set<int>>(coversEdges, coversFinality);
	}

	// 最小化DFA
	pair<DFAedges, set<int>> getMinimizedDfa(DFAedges dfaEdges, set<int> dfaFinality)
	{
		pair<DFAedges, set<int>> result1({dfaEdges, dfaFinality});
		result1 = getEmptyStatesEliminatedDfa(result1.first, result1.second);
		result1 = getEqualStatesCombinedDfa(result1.first, result1.second);
		return result1;
		// return getEmptyStatesEliminatedDfa(result1.first, result1.second);
	}

	// 邻接表转NFA邻接矩阵
	NFAedges edgeTable2NFAedges(int numStates, EdgeTable edgeTable)
	{
		NFAedges nfaEdges(numStates);

		for (Edge edge : edgeTable) {
			nfaEdges[edge.from][edge.eChar].push_back(edge.to);
		}
		return nfaEdges;
	}

	// 消除DFA不可达状态
	pair<DFAedges, set<int>> getEmptyStatesEliminatedDfa(DFAedges dfaEdges, set<int> dfaFinality)
	{
		vector<bool> visited(dfaEdges.size(), false);
		queue<int> front;
		front.push(0);
		visited[0] = true;

		// 广度优先搜索，得到可访问性
		while (front.size() != 0) {
			int current = front.front();
			front.pop();

			for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
				if (charIdx == EMPTY_CHAR) {
					continue;
				}
				if (dfaEdges[current][charIdx] != ERROR_STATE) {
					int next = dfaEdges[current][charIdx];
					if (!visited[next]) {
						front.push(next);
						visited[next] = true;
					}
				}
			}
		}

		DFAedges resDfa;
		set<int> resDfaFinality;
		// 弹出不可访问状态后，状态值重映射
		map<int, int> newStateIdx;
		for (int i = 0, j = 0; (uint32_t)i < dfaEdges.size(); i++, j++) {
			while ((uint32_t)i < dfaEdges.size() && !visited[i]) {
				i++;
			}
			if ((uint32_t)i >= dfaEdges.size()) {
				break;
			}
			newStateIdx[i] = j;
			
			resDfa.push_back(dfaEdges[i]);
			if (dfaFinality.count(i)) {
				resDfaFinality.insert(j);
			}
		}
		// 根据映射对边进行替换
		for (DFAitem &item : resDfa) {
			for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
				if (charIdx == EMPTY_CHAR) {
					continue;
				}
				if (item[charIdx] != ERROR_STATE) {
					item[charIdx] = newStateIdx[item[charIdx]];
				}
			}
		}

		return pair<DFAedges, set<int>>(resDfa, resDfaFinality);
	}


	// 合并DFA等价状态
	pair<DFAedges, set<int>> getEqualStatesCombinedDfa(DFAedges dfaEdges, set<int> dfaFinality)
	{
		int numId = 1;
		vector<int> splitedId(dfaEdges.size());

		// 按是否可终结作初次划分
		for (int i = 0; (uint32_t)i < dfaEdges.size(); i++) {
			if (dfaFinality.count(i) == dfaFinality.count(0)) {
				splitedId[i] = 0;
			} else {
				numId = 2;
				splitedId[i] = 1;
			}
		}

		// 按后继状态是否属于同一集合进行进一步划分
		while (true) {
			bool isUpdated = false; // 是否有更新过

			for (int id = 0; id < numId; id++) {
				bool flag = false; // 标记id对应的集合是否发生分裂

				auto it1 = find(splitedId.begin(), splitedId.end(), id);
				if (it1 == splitedId.end()) {
					continue;
				}
				auto it2 = find(it1 + 1, splitedId.end(), id);
				int i1 = it1 - splitedId.begin();
				int i2 = it2 - splitedId.begin();

				// 逐一比较后继状态是否属于同一集合
				while (it2 != splitedId.end()) {
					for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
						if (charIdx == EMPTY_CHAR) {
							continue;
						}
						int nextStateIdx1 = dfaEdges[i1][charIdx];
						int nextStateIdx2 = dfaEdges[i2][charIdx];
						if (splitedId[nextStateIdx1] != splitedId[nextStateIdx2]) {
							flag = true;
							break;
						}
					}
					if (flag) {
						break;
					}

					it1 = it2;
					it2 = find(it2 + 1, splitedId.end(), id);
					i1 = it1 - splitedId.begin();
					i2 = it2 - splitedId.begin();
				}

				// 后继状态不属于同一集合，发生分裂，并重新检测该集合
				if (flag) {
					splitedId[i2] = numId++;
					id--;
					continue;
				}
			}

			// 只要有一次更新都必须从头再试一遍
			if (!isUpdated) {
				break;
			}
		}

		// 整理集合
		vector<vector<int>> splitedStates(numId);
		for (int i = 0; (uint32_t)i < dfaEdges.size(); i++) {
			splitedStates[splitedId[i]].push_back(i);
		}
		

		DFAedges resDfa;
		set<int> resDfaFinality;
		// 用等价集合编号代替原先元素
		for (int i = 0; i < numId; i++) {
			resDfa.push_back(dfaEdges[splitedStates[i][0]]);
			for (int charIdx = 0; charIdx < CHARSET_SIZE; charIdx++) {
				if (charIdx == EMPTY_CHAR) {
					continue;
				}
				if (resDfa.back()[charIdx] != ERROR_STATE) {
					int nextStateIdxSplitedId = splitedId[resDfa.back()[charIdx]];
					resDfa.back()[charIdx] = nextStateIdxSplitedId;
				}
			}

			if (dfaFinality.count(splitedStates[i][0]) != 0) {
				resDfaFinality.insert(i);
			}
		}

		return pair<DFAedges, set<int>>(resDfa, resDfaFinality);
	}

	// 扩张覆盖片选择（epsilon-闭包法）
	void setCoverExpanded(set<int> &cover, NFAedges nfa)
	{
		queue<int> q;
		for (int stateIdx : cover) {
			q.push(stateIdx);
		}
		while (q.size()) {
			int stateIdx = q.front();
			q.pop();
			for (int nextStateIdx : nfa[stateIdx][EMPTY_CHAR]) {
				if (cover.count(nextStateIdx) == 0) {
					cover.insert(nextStateIdx);
					q.push(nextStateIdx);
				}
			}
		}
	}

	// 求后继覆盖片
	set<int> getNextCover(set<int> cover, NFAedges nfa, int charIdx)
	{
		set<int> nextCover;
		for (int stateIdx : cover) {
			for (int nextStateIdx : nfa[stateIdx][charIdx]) {
				nextCover.insert(nextStateIdx);
			}
		}
		setCoverExpanded(nextCover, nfa);

		return nextCover;
	}

	// 测试nfa转dfa
	void testNfa2dfa()
	{
		NFAedges nfa;
		set<int> nfaFinality;

		nfa = getNewNFAedges(3, {
			{'a', 0, 1}, 
			{'b', 1, 1}, 
			{'b', 1, 2}
		});
		nfaFinality.insert(2);

		printNFA(nfa);

		pair<DFAedges, set<int>> result = nfa2dfa(nfa, nfaFinality);
		DFAedges dfaEdges = result.first;
		set<int> dfaFinality = result.second;

		printDFA(dfaEdges);
	}

	// 测试去除dfa不可达状态
	void testEliminateEmptyState()
	{
		DFAedges dfaEdges;
		set<int> dfaFinality;

		dfaEdges.push_back(array<int, CHARSET_SIZE>({ -1, 2, -1 }));
		dfaEdges.push_back(array<int, CHARSET_SIZE>({ -1, 0, -1 }));
		dfaEdges.push_back(array<int, CHARSET_SIZE>({ -1, -1, 0 }));
		dfaFinality.insert(0);

		printDFA(dfaEdges);

		pair<DFAedges, set<int>> result = getEmptyStatesEliminatedDfa(dfaEdges, dfaFinality);
		DFAedges dfa2 = result.first;
		set<int> dfaFinality2 = result.second;
		
		printDFA(dfa2);
	}

	// 测试合并等价状态
	void testSimplifyEqualState()
	{
		DFAedges dfaEdges = getNewDFAedges({
			{'a', 'b'}, 
			{ 1,   2 }, 
			{ 1,   3 }, 
			{ 1,   2 }, 
			{ 1,   4 }, 
			{ 1,   2 }, 
		});
		set<int> dfaFinality({4});

		printDFA(dfaEdges);

		pair<DFAedges, set<int>> result = getEqualStatesCombinedDfa(dfaEdges, dfaFinality);
		DFAedges dfaEdges2 = result.first;
		set<int> dfaFinality2 = result.second;

		printDFA(dfaEdges2);
	}

	
}
#include "syntax.h"
#include <algorithm>
using namespace std;

namespace compiler {
	
	pair<DFAedges, map<int, int>> getDFAintegrated(vector<DFAedges> dfas, vector<set<int>> dfaFinalities)
	{
		vector<EdgeTable> edgeTables;
		for (DFAedges dfa : dfas) {
			edgeTables.push_back(dfaEdges2EdgeTable(dfa));
		}

		EdgeTable nfaEdgeTable;
		map<int, int> nfaFinalities;

		// 更新规则很简单，就是把每个状态的重编号，把边集求合
		// states = {0, dfa0.states, dfa1.states, ...}
		int numStateAdded = 1;
		for (int i = 0; i < dfas.size(); i++) {
			// 更新边集合
			nfaEdgeTable.push_back({EMPTY_CHAR, 0, numStateAdded});
			for (Edge &edge : edgeTables[i]) {
				nfaEdgeTable.push_back({
					edge.eChar, 
					edge.from + numStateAdded, 
					edge.to + numStateAdded
				});
			}
			// 更新终结状态集合
			for (int elem : dfaFinalities[i]) {
				nfaFinalities[elem + numStateAdded] = i;
			}

			numStateAdded += dfas[i].size();
		}

		NFAedges nfa = edgeTable2NFAedges(numStateAdded, nfaEdgeTable);
		DFAedges coversEdges;
		map<int, int> coversFinalities;
		// nfa2dfa(大体相同)
		{
			vector<set<int>> covers;

			// 默认空行: 全-1
			DFAitem emptyCoversEdgesRow;
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

				// 标记是否为终结状态
				for (int stateIdx : cover) {
					if (nfaFinalities.count(stateIdx)) {
						// 若有冲突则优先采用靠前的dfa接受
						if (coversFinalities.count(i) == 0) {
							coversFinalities[i] = nfaFinalities[stateIdx];
						}
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
		}
		return pair<DFAedges, map<int, int>>(coversEdges, coversFinalities);
	}

	vector<Token> getTokens(DFAedges dfa, map<int, int> dfaFinalities, const string &src)
	{
		vector<Token> tokens;
		int st = 0;
		int state = 0;

		for (int i = 0; i < src.size(); i++) {
			int c = src[i];
			int nextState = dfa[state][c];

			if (nextState == ERROR_STATE) {
				if (dfaFinalities.count(state)) {
					// 匹配一个token
					int type = dfaFinalities[state];
					tokens.push_back({type, st, i});
					st = i;
					state = 0;
					i--;
				} else {					throw "syntax error";
				}
			} else {
				state = nextState;
			}
		}

		if (dfaFinalities.count(state)) {
			// 匹配一个token
			int type = dfaFinalities[state];
			tokens.push_back({type, st, (int)src.size()});
		} else {			throw "syntax error";
		}

		return tokens;
	}

}

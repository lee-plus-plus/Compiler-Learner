#include "grammar.h"
#include "nfa2dfa.h"
#include <algorithm>
#include <stack>
using namespace std;

namespace compiler {

	// 求首符集
	map<int, set<int>> getFirstSet(set<int> symbolset, vector<Production> productions)
	{
		map<int, set<int>> firstSet;
		// follow(a) = {a}
		for (int c = 0; c < CHARSET_SIZE; c++) {
			firstSet[c] = {c};
		}
		// follow(A) = {}
		for (int symbol : symbolset) {
			firstSet[symbol] = {};
		}

		for (Production prod : productions) {
			// prod: X -> ...
			if (prod.right.size() > 0 && isTerminal(prod.right[0])) {
				// X -> a... ==> first(X) += {a}
				firstSet[prod.symbol].insert(prod.right[0]);
			} else if (prod.right.size() == 0) {
				// X -> epsilon ==> first(X) += {epsilon}
				firstSet[prod.symbol].insert(EMPTY_CHAR);
			}
		}

		for (Production prod : productions) {
			// prod: X -> ... (not empty)
			if (prod.right.size() == 0) {
				continue;
			}

			for (int i = 0; i < prod.right.size(); i++) {
				// prod: X -> Y0 Y1...Yi γ, epsilon ∈ first(Y0),...,first(Yi)
				// ==> first(X) += first(γ)
				int y = prod.right[i];
				set<int> tempSet = firstSet[y];
				tempSet.erase(EMPTY_CHAR);
				firstSet[prod.symbol].insert(tempSet.begin(), tempSet.end());

				if (firstSet[y].count(EMPTY_CHAR) == 0) {
					break;
				} else {
					// prod: X -> Y0 Y1...Yi, epsilon ∈ first(Y0),...,first(Yi)
					// ==> first(X) += {epsilon}
					if (i == prod.right.size() - 1) {
						firstSet[prod.symbol].insert(EMPTY_CHAR);
					}
				}
			}
		}

		return firstSet;	
	}

	// 求随符集
	map<int, set<int>> getFollowSet(set<int> symbolset, vector<Production> productions, map<int, set<int>> firstSet)
	{
		map<int, set<int>> followSet;
		// init prod: S -> ... ==> follow(S) += {#}
		followSet[productions[0].symbol].insert(END_CHAR);
		
		for (Production prod : productions) {
			// prod: B -> ...

			for (int i = 0; i < prod.right.size(); i++) {
				if (symbolset.count(prod.right[i])) {
					if (i != prod.right.size() - 1) {
						// B -> αAβ ==> follow(A) += {first(β) - epsilon}
						int A = prod.right[i];
						int b = prod.right[i + 1];
						set<int> tempSet = firstSet[b];
						tempSet.erase(EMPTY_CHAR);
						followSet[A].insert(tempSet.begin(), tempSet.end());
					} else {
						// B -> αA ==> follow(A) += {follow(B)}
						int A = prod.right[i];
						int B = prod.symbol;
						followSet[A].insert(followSet[B].begin(), followSet[B].end());
					}
				}
			}
		}

		return followSet;
	}

	// lr(0)分析
	pair<vector<set<ProductionItem>>, EdgeTable> getLR0dfa(set<int> symbolset, vector<Production> prods)
	{
		vector<set<ProductionItem>> covers;
		EdgeTable edgeTable;

		set<ProductionItem> initCover;
		initCover.insert({prods[0].symbol, prods[0].right, 0});
		setLR0CoverExpanded(initCover, prods);
		covers.push_back(initCover);

		for (int i = 0; i < covers.size(); i++) {
			map<int, set<ProductionItem>> nextCovers;

			for (ProductionItem prodItem : covers[i]) {
				// A -> α·Bβ ==B=> A -> αB·β
				if (prodItem.dot < prodItem.right.size()) {
					int c = prodItem.right[prodItem.dot];
					if (nextCovers.count(c) == 0) {
						nextCovers[c] = {};
					}
					nextCovers[c].insert({
						prodItem.symbol, prodItem.right, prodItem.dot + 1
					});
				}
			}

			for (auto elem : nextCovers) {
				set<ProductionItem> nextCover = elem.second;
				setLR0CoverExpanded(nextCover, prods);

				bool isUnique = true;
				int tgtIdx = -1;
				for (int j = 0; j < covers.size(); j++) {
					if (nextCover == covers[j]) {
						isUnique = false;
						tgtIdx = j;
						break;
					}
				}

				if (isUnique) {
					// printf("[%d,%d,%d]", elem.first, i, (int)covers.size() - 1);
					covers.push_back(nextCover);
					edgeTable.push_back({elem.first, i, (int)covers.size() - 1});
				} else {
					// printf("[%d,%d,%d]", elem.first, i, tgtIdx);
					edgeTable.push_back({elem.first, i, tgtIdx});
				}
			}
		}

		return pair<vector<set<ProductionItem>>, EdgeTable>(covers, edgeTable);
	}

	// 扩张覆盖片选择（epsilon-闭包法）
	void setLR0CoverExpanded(set<ProductionItem> &cover, vector<Production> prods)
	{
		queue<ProductionItem> q;
		for (ProductionItem prodItem : cover) {
			q.push(prodItem);
		}
		while (q.size()) {
			ProductionItem prodItem = q.front();
			q.pop();

			if (prodItem.dot == prodItem.right.size()) {
				continue;
			}
			// A -> α·Bβ, B -> ... ==ε=> cover += (B -> ·...)
			for (Production prod : prods) {
				if (prod.symbol == prodItem.right[prodItem.dot]) {
					ProductionItem nextProdItem({prod.symbol, prod.right, 0});
					if (cover.count(nextProdItem) == 0) {
						cover.insert(nextProdItem);
						q.push(nextProdItem);
					}
				}
			}
		}
	}

	// lr(1)分析
	pair<vector<set<ProductionLR1Item>>, EdgeTable> getLR1dfa(set<int> symbolset, vector<Production> prods)
	{
		vector<set<ProductionLR1Item>> covers;
		EdgeTable edgeTable;

		map<int, set<int>> firstSet = getFirstSet(symbolset, prods);

		set<ProductionLR1Item> initCover;
		initCover.insert({prods[0].symbol, prods[0].right, 0, END_CHAR});
		setLR1CoverExpanded(initCover, firstSet, prods);
		covers.push_back(initCover);

		for (int i = 0; i < covers.size(); i++) {
			map<int, set<ProductionLR1Item>> nextCovers;

			for (ProductionLR1Item prodItem : covers[i]) {
				// (A -> α·Bβ, s) ==B=> (A -> αB·β, s)
				if (prodItem.dot < prodItem.right.size()) {
					int c = prodItem.right[prodItem.dot];
					if (nextCovers.count(c) == 0) {
						nextCovers[c] = {};
					}
					nextCovers[c].insert({
						prodItem.symbol, prodItem.right, 
						prodItem.dot + 1, prodItem.search
					});
				}
			}

			for (auto elem : nextCovers) {
				set<ProductionLR1Item> nextCover = elem.second;
				setLR1CoverExpanded(nextCover, firstSet, prods);

				bool isUnique = true;
				int tgtIdx = -1;
				for (int j = 0; j < covers.size(); j++) {
					if (nextCover == covers[j]) {
						isUnique = false;
						tgtIdx = j;
						break;
					}
				}

				if (isUnique) {
					// printf("[%d,%d,%d]", elem.first, i, (int)covers.size() - 1);
					covers.push_back(nextCover);
					edgeTable.push_back({
						elem.first, i, (int)covers.size() - 1
					});
				} else {
					// printf("[%d,%d,%d]", elem.first, i, tgtIdx);
					edgeTable.push_back({elem.first, i, tgtIdx});
				}
			}
		}

		return pair<vector<set<ProductionLR1Item>>, EdgeTable>(covers, edgeTable);
	}

	// 扩张覆盖片选择（epsilon-闭包法）
	void setLR1CoverExpanded(set<ProductionLR1Item> &cover, map<int, set<int>> firstSet, vector<Production> prods)
	{
		queue<ProductionLR1Item> q;
		for (ProductionLR1Item prodItem : cover) {
			q.push(prodItem);
		}
		while (q.size()) {
			ProductionLR1Item prodItem = q.front();
			q.pop();

			if (prodItem.dot == prodItem.right.size()) {
				continue;
			}
			if (isTerminal(prodItem.right[prodItem.dot])) {
				continue;
			}

			for (Production prod : prods) {
				if (prod.symbol != prodItem.right[prodItem.dot]) {
					continue;
				}

				if (prodItem.dot + 1 < prodItem.right.size()) {
					// (A -> α·Bβ, s) ==ε=> (B -> ·..., first(β))
					set<int> nextSet = firstSet[prodItem.right[prodItem.dot + 1]];
					for (int c : nextSet) {
						ProductionLR1Item nextProdItem({
							prod.symbol, prod.right, 0, c
						});
						if (cover.count(nextProdItem) == 0) {
							cover.insert(nextProdItem);
							q.push(nextProdItem);
						}
					}
				} else {
					// (A -> α·B, s) ==ε=> (B -> ·..., s)
					ProductionLR1Item nextProdItem({
						prod.symbol, prod.right, 0, prodItem.search
					});
					if (cover.count(nextProdItem) == 0) {
						cover.insert(nextProdItem);
						q.push(nextProdItem);
					}
				}
			}
		}
	}

	// LR(1) analyze table
	map<pair<int, int>, Action> getLR1table(vector<Production> prods, vector<set<ProductionLR1Item>> covers, EdgeTable edgeTable)
	{
		map<pair<int, int>, Action> analyzeTable;

		// edges ==> ACTION and GOTO
		for (Edge edge : edgeTable) {
			if (isTerminal(edge.eChar)) {
				// s1 ——a-> s2 ==> action[s1, a] = s2
				analyzeTable[{edge.from, edge.eChar}] = {Action::ACTION, edge.to};
			} else {
				// s1 ——A-> s2 ==> goto[s1, A] = s2
				analyzeTable[{edge.from, edge.eChar}] = {Action::GOTO, edge.to};
			}
		}

		// node ==> REDUCE and ACCEPT
		for (int i = 0; i < covers.size(); i++) {
			for (ProductionLR1Item item : covers[i]) {
				// s1: (A -> ...·, s), r2: A -> ... ==> reduce[s1, s] = r2
				if (item.dot == item.right.size()) {
					for (int j = 0; j < prods.size(); j++) {
						if (prods[j].symbol == item.symbol && prods[j].right == item.right) {
							analyzeTable[{i, item.search}] = {Action::REDUCE, j};
							break;
						}
					}
				}
				// s1: (S -> ...·, #) ==> accept[s1, #]
				if (item.symbol == prods[0].symbol && item.dot == item.right.size()) {
					analyzeTable[{i, item.search}] = {Action::ACCEPT, 0};
				}
			}
		}

		return analyzeTable;
	}

	// LR(1) analyze, return grammar tree
	GrammarNode *getLR1grammarTree(vector<Production> prods, map<pair<int, int>, Action> analyzeTable, string src)
	{
		if (src[src.size() - 1] != END_CHAR) {
			src += END_CHAR;
		}

		// stack of states
		stack<int> states;
		stack<GrammarNode *> stateNodes;
		states.push(0);

		// stack of symbols (unnecessary)
		stack<int> symbols;


		for (int i = 0; ; ) {
			// print stack
			{
				stack<int> tempSymbols;
				printf("s=%d \tnext=%c \t", states.top(), src[i]);
				while (symbols.size()) {
					tempSymbols.push(symbols.top());
					symbols.pop();
				}
				while (tempSymbols.size()) {
					symbols.push(tempSymbols.top());
					printSymbol(tempSymbols.top());
					tempSymbols.pop();
				}
				printf("\n");
			}

			// look src[i], state => next_state, action
			bool isError =  analyzeTable.count({states.top(), src[i]}) == 0;
			if (isError) {
				ERR_LOG("syntax error: unexpected symbol '%c'\n", src[i]);
			}

			Action action = analyzeTable[{states.top(), src[i]}];
			switch (action.type) {
				case Action::ACTION: {
					symbols.push(src[i]);
					states.push(action.tgt);
					stateNodes.push(new GrammarNode({{}, src[i]}));
					i++;
					break;
				}
				case Action::REDUCE: {
					Production r = prods[action.tgt];
					stack<GrammarNode *> temp;
					for (int j = 0; j < r.right.size(); j++) {
						temp.push(stateNodes.top());
						symbols.pop();
						states.pop();
						stateNodes.pop();
					}

					bool isError2 =  analyzeTable.count({states.top(), r.symbol}) == 0;
					if (isError2) {
						ERR_LOG(
							"syntax error: unexpected symbol '\e[0;36m%c\e[0m'\n", 
							r.symbol - CHARSET_SIZE
						);
					}
					Action action2 = analyzeTable[{states.top(), r.symbol}];
					symbols.push(r.symbol);
					states.push(action2.tgt);

					GrammarNode *nextNode = new GrammarNode({{}, r.symbol});
					while (temp.size()) {
						nextNode->children.push_back(temp.top());
						temp.pop();
					}
					stateNodes.push(nextNode);
					break;
				}
				case Action::ACCEPT: {
					break;
				}
				default: {
					ERR_LOG("syntax error: unexpected symbol '%c'\n", src[i]);
					break;
				}
			}				

		}

		return stateNodes.top();
	}

}

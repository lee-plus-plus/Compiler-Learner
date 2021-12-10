#include "defs.h"
using namespace std;

namespace compiler {

	inline int toNonterminal(int symbol)
	{
		return isNonterminal(symbol) ? symbol : symbol + CHARSET_SIZE;
	}
	inline int toTerminal(int symbol)
	{
		return isTerminal(symbol) ? symbol : symbol - CHARSET_SIZE;
	}

	inline bool isTerminal(int symbol)
	{
		return (0 <= symbol && symbol < CHARSET_SIZE);
	}

	inline bool isNonterminal(int symbol)
	{
		return (symbol >= CHARSET_SIZE);
	}

	bool Production::operator<(const Production &p) const {
		return (
			symbol < p.symbol || (symbol == p.symbol && (
				right < p.right
			))
		);
	}

	bool Production::operator==(const Production &p) const {
		return (symbol == p.symbol && val == p.val);
	}

	bool ProductionItem::operator<(const ProductionItem &p) const {
		return (
			symbol < p.symbol || (symbol == p.symbol && (
				right < p.right || (right == p.right && (
					dot < p.dot
				))
			))
		);
	}

	bool ProductionItem::operator==(const ProductionItem &p) const {
		return (symbol == p.symbol && val == p.val && dot == p.dot);
	}

	bool ProductionLR1Item::operator<(const ProductionLR1Item &p) const {
		return (
			symbol < p.symbol || (symbol == p.symbol && (
				right < p.right || (right == p.right && (
					dot < p.dot || (dot == p.dot && (
						search == p.search
					))
				))
			))
		);
	}

	bool ProductionLR1Item::operator==(const ProductionLR1Item &p) const {
		return (
			symbol == p.symbol && val == p.val && 
			dot == p.dot && search == p.search
		);
	}

}
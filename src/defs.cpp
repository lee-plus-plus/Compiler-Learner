#include "defs.h"
using namespace std;

namespace compiler {

	bool Production::operator<(const Production &p) const {
		return (
			symbol < p.symbol || (symbol == p.symbol && (
				right < p.right
			))
		);
	}

	bool Production::operator==(const Production &p) const {
		return (symbol == p.symbol && right == p.right);
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
		return (symbol == p.symbol && right == p.right && dot == p.dot);
	}

	bool ProductionLR1Item::operator<(const ProductionLR1Item &p) const {
		return (
			symbol < p.symbol || (symbol == p.symbol && (
				right < p.right || (right == p.right && (
					dot < p.dot
				))
			))
		);
	}

	bool ProductionLR1Item::operator==(const ProductionLR1Item &p) const {
		return (
			symbol == p.symbol && right == p.right && 
			dot == p.dot && search == p.search
		);
	}

}
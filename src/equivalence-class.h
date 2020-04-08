#pragma once

#include "resource-record.h"

struct EC {
	boost::optional<std::vector<NodeLabel>> excluded;
	vector<NodeLabel> name;
	std::bitset<RRType::N> rrTypes;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& name;
		ar& rrTypes;
		ar& excluded;
	}
public:
	string ToString() const;
	bool operator== (const EC&) const;
};
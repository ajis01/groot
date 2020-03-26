#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/adjacency_list.hpp>	
#include <boost/graph/graphviz.hpp>	
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "resource_record.h"
#include "utils.h"

using namespace std;

#define kHashMapThreshold 500

//Label Graph
struct LabelVertex {
	Label name;
	int16_t	len = -1;
	std::bitset<RRType::N> rrTypesAvailable;
	std::vector<tuple<int, int>> zoneIdVertexId;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& name;
		ar& len;
		ar& rrTypesAvailable;
		ar& zoneIdVertexId;
	}
};

enum EdgeType {
	normal = 1,
	dname = 2
};

struct LabelEdge {
	EdgeType type;
private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& type;
	}
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, LabelVertex, LabelEdge> LabelGraph;

//Some typedefs for simplicity
typedef boost::graph_traits<LabelGraph>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<LabelGraph>::edge_descriptor EdgeDescriptor;
typedef boost::graph_traits<LabelGraph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<LabelGraph>::edge_iterator EdgeIterator;

typedef boost::unordered_map<Label, VertexDescriptor> LabelMap;
extern std::unordered_map<VertexDescriptor, LabelMap> gDomainChildLabelMap;

//void LabelGraphBuilder(vector<ResourceRecord>&, LabelGraph&, const VertexDescriptor);
void LabelGraphBuilder(ResourceRecord&, LabelGraph&, const VertexDescriptor, int&, int);

//Equivalence Classes
struct EC {
	boost::optional<std::vector<Label>> excluded;
	vector<Label> name;
	std::bitset<RRType::N> rrTypes;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& name;
		ar& rrTypes;
		ar& excluded;
	}
};

typedef struct EC EC;
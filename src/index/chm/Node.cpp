#include "Node.hpp"

namespace chm {
	Node::Node() : distance(0), id(0) {}
	Node::Node(const float distance, const uint id) : distance(distance), id(id) {}
}

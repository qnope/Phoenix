#include "GroupNode.h"

namespace phx {

void BaseGroupNode::addChild(Node node) { m_nodes.push_back(std::move(node)); }

void BaseGroupNode::processChildren(NodeVisitor &visitor) {
    for (auto node : m_nodes)
        node.accept(visitor);
}

void GroupNode::accept(NodeVisitor &visitor) {
    if (visitor.visit(*this)) {
        processChildren(visitor);
    }
}

} // namespace phx

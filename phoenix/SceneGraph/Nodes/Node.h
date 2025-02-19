#pragma once

#include <cassert>
#include <memory>
#include <typeindex>

#include "../Visitors/NodeVisitor.h"
#include "../Visitors/WithResultVisitor.h"

namespace phx {

class Node {
    class NodeConcept {
      public:
        virtual void accept(NodeVisitor &visitor) = 0;
        virtual void *ptr() = 0;

        virtual ~NodeConcept() = default;
    };

    template <typename ConcreteNode>
    class NodeModel : public NodeConcept {
        static_assert(std::is_base_of_v<AbstractNode, ConcreteNode>, "ConcreteNode must be derived from AbstractNode");

      public:
        NodeModel(ConcreteNode node) noexcept : m_node{std::move(node)} {}

        void *ptr() override { return std::addressof(m_node); }

        void accept(NodeVisitor &visitor) override { m_node.accept(visitor); }

      private:
        ConcreteNode m_node;
    };

  public:
    template <typename ConcreteNode>
    Node(ConcreteNode node) noexcept :
        m_typeIndex{typeid(ConcreteNode)}, m_node{std::make_shared<NodeModel<ConcreteNode>>(std::move(node))} {}

    std::type_index typeIndex() const noexcept { return m_typeIndex; }

    template <typename ConcreteNode>
    ConcreteNode *get() {
        assert(m_node);
        assert(m_typeIndex == typeid(ConcreteNode));
        return static_cast<ConcreteNode *>(m_node->ptr());
    }

    template <typename Visitor>
    auto accept(Visitor &&visitor) {
        assert(m_node);
        m_node->accept(visitor);

        if_constexpr(is_with_result_visitor(visitor)) { return visitor.takeResult(); }
    }

  private:
    std::type_index m_typeIndex;
    std::shared_ptr<NodeConcept> m_node;
};

} // namespace phx

#pragma once

namespace phx {
class AbstractNode {
protected:
  virtual ~AbstractNode() noexcept = default;
};

class AbstractLeafNode : public AbstractNode {
public:
  template <typename Visitor> void accept(Visitor &visitor) {
    visitor.visit(*this);
  }
};
} // namespace phx

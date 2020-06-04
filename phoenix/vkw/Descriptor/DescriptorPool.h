#pragma once

#include "DescriptorPoolList.h"
#include <cassert>
#include <memory>
#include <typeindex>

namespace phx {
class DescriptorPool {
  class Concept {
  public:
    Concept(std::type_index type) noexcept : m_type{type} {}

    std::type_index type() const noexcept { return m_type; }
    virtual void *ptr() noexcept = 0;

  private:
    std::type_index m_type;
  };

  template <typename T> class Model : public Concept {
    static_assert(is_descriptor_pool_list(ltl::type_v<T>),
                  "T must be a descriptor pool list");

  public:
    Model(T descriptorPoolList) noexcept
        : Concept{typeid(T)}, //
          m_descriptorPoolList{std::move(descriptorPoolList)} {}

    void *ptr() noexcept override {
      return std::addressof(m_descriptorPoolList);
    }

  private:
    T m_descriptorPoolList;
  };

public:
  template <typename T>
  DescriptorPool(T poolList) noexcept
      : m_ptr{std::make_shared<Model<T>>(std::move(poolList))} {}

  std::type_index type() const noexcept { return m_ptr->type(); }

  template <typename Layout>
  auto allocate(decltype_t(Layout::type_list) values) {
    using List = DescriptorPoolList<Layout>;
    assert(m_ptr->type() == typeid(List));
    auto *ptr = static_cast<List *>(m_ptr->ptr());
    return values(
        [ptr](auto... xs) { return ptr->allocate(std::move(xs)...); });
  }

private:
  std::shared_ptr<Concept> m_ptr;
};
} // namespace phx

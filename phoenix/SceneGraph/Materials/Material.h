#pragma once

#include <cassert>
#include <memory>
#include <typeindex>
#include <utility>

#include <vkw/vulkan.h>

namespace phx {
class Material {
  class Concept {
  public:
    Concept(std::type_index type, std::type_index layoutType,
            vk::DescriptorSet descriptorSet) noexcept
        : m_type{type},             //
          m_layoutType{layoutType}, //
          m_descriptorSet{descriptorSet} {}

    std::type_index type() const noexcept { return m_type; }
    std::type_index layoutType() const noexcept { return m_layoutType; }
    vk::DescriptorSet descriptorSet() const noexcept { return m_descriptorSet; }

    virtual void *ptr() noexcept = 0;

  private:
    std::type_index m_type;
    std::type_index m_layoutType;
    vk::DescriptorSet m_descriptorSet;
  };

  template <typename T> class Model : public Concept {
  public:
    Model(T material)
        : Concept{typeid(T), material.layoutType(), material.descriptorSet()} {}

    void *ptr() noexcept override { return std::addressof(m_material); }

  private:
    T m_material;
  };

public:
  template <typename T>
  Material(T material) noexcept
      : m_ptr{std::make_shared<Model<T>>(std::move(material))} {}

  std::type_index type() const noexcept { return m_ptr->type(); }
  std::type_index layoutType() const noexcept { return m_ptr->layoutType(); }
  vk::DescriptorSet descriptorSet() const noexcept {
    return m_ptr->descriptorSet();
  }

  template <typename T> T *get() {
    assert(type() != typeid(T));
    return static_cast<T *>(m_ptr->ptr());
  }

private:
  std::shared_ptr<Concept> m_ptr;
};
} // namespace phx

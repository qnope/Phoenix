#pragma once

#include "With_Pipeline/WithBuffer.h"
#include "vulkan.h"
#include <glm/glm.hpp>
#include <ltl/Tuple.h>

namespace phx {
namespace details {
constexpr auto getCardinality(ltl::type_t<glm::vec2>) { return 2_n; }
constexpr auto getCardinality(ltl::type_t<glm::vec3>) { return 3_n; }
constexpr auto getCardinality(ltl::type_t<glm::vec4>) { return 4_n; }

constexpr auto getLocationSize(ltl::type_t<glm::vec2>) { return 1u; }
constexpr auto getLocationSize(ltl::type_t<glm::vec3>) { return 1u; }
constexpr auto getLocationSize(ltl::type_t<glm::vec4>) { return 1u; }

constexpr auto getFormat(ltl::type_t<glm::vec2>) {
  return vk::Format::eR32G32Sfloat;
}

constexpr auto getFormat(ltl::type_t<glm::vec3>) {
  return vk::Format::eR32G32B32Sfloat;
}

constexpr auto getFormat(ltl::type_t<glm::vec4>) {
  return vk::Format::eR32G32B32A32Sfloat;
}

#define MAKE_ATTRIBUTE(Name, name)                                             \
  template <typename Vec> struct Name {                                        \
    static constexpr auto byte_number = sizeof(Vec);                           \
    static constexpr auto format = getFormat(ltl::type_v<Vec>);                \
    static constexpr auto cardinality = getCardinality(ltl::type_v<Vec>);      \
    static constexpr auto locationSize = getLocationSize(ltl::type_v<Vec>);    \
    template <typename... Xs> Name(Xs... xs) : name(xs...) {}                  \
    Vec name;                                                                  \
  }

MAKE_ATTRIBUTE(Color, color);
MAKE_ATTRIBUTE(Normal, normal);
MAKE_ATTRIBUTE(Position, position);
MAKE_ATTRIBUTE(TexCoord, texCoord);
MAKE_ATTRIBUTE(Tangeant, tangeant);
MAKE_ATTRIBUTE(BiTangeant, biTangeant);

} // namespace details
using Position2D = details::Position<glm::vec2>;
using Position3D = details::Position<glm::vec3>;
using Position4D = details::Position<glm::vec4>;

using RgbColor = details::Color<glm::vec3>;
using RgbaColor = details::Color<glm::vec4>;

template <typename... types> struct Vertex : types... {
  static constexpr auto all_types = ltl::type_list_v<types...>;

  Vertex(types... ts) : types(ts)... {}

  template <typename Index>
  static constexpr auto getAttributeDescription(uint32_t binding, Index index) {
    using current = decltype_t(all_types[index]);
    auto indexer = ltl::build_index_sequence(index);

    return indexer([binding](auto... index) {
      uint32_t location =
          (0u + ... + (decltype_t(all_types[index])::locationSize));
      uint32_t offset =
          (0u + ... + (decltype_t(all_types[index])::byte_number));
      return vk::VertexInputAttributeDescription(location, binding,
                                                 current::format, offset);
    });
  }

  template <int N>
  static constexpr auto getBindingDescription(ltl::number_t<N> n) {
    auto indexer = ltl::build_index_sequence(all_types.length);

    return indexer([n](auto... index) {
      return BindingDescription{n, ltl::type_v<Vertex>,
                                (types::byte_number + ...),
                                getAttributeDescription(n.value, index)...};
    });
  }
};

using Colored2DVertex = Vertex<Position2D, RgbColor>;

} // namespace phx

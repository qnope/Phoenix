#pragma once

#include "With_Pipeline/WithBuffer.h"
#include "vulkan.h"
#include <ltl/operator.h>
#include <ltl/tuple_algos.h>

namespace phx {
namespace details {
constexpr auto getCardinality(ltl::type_t<glm::vec2>) { return 2_n; }
constexpr auto getCardinality(ltl::type_t<glm::vec3>) { return 3_n; }
constexpr auto getCardinality(ltl::type_t<glm::vec4>) { return 4_n; }

constexpr auto getLocationSize(ltl::type_t<glm::vec2>) { return 1_n; }
constexpr auto getLocationSize(ltl::type_t<glm::vec3>) { return 1_n; }
constexpr auto getLocationSize(ltl::type_t<glm::vec4>) { return 1_n; }

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
    static constexpr auto byte_number = number_v<sizeof(Vec)>;                 \
    static constexpr auto format = getFormat(ltl::type_v<Vec>);                \
    static constexpr auto cardinality = getCardinality(ltl::type_v<Vec>);      \
    static constexpr auto location_size = getLocationSize(ltl::type_v<Vec>);   \
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

using TexCoord = details::TexCoord<glm::vec2>;

template <typename... Types> struct Vertex : Types... {
  static constexpr auto types = ltl::type_list_v<Types...>;

  Vertex(Types... ts) : Types(ts)... {}

  template <typename Index>
  static constexpr auto getAttributeDescription(uint32_t binding, Index index) {
    auto format = decltype_t(types[index])::format;

    return vk::VertexInputAttributeDescription(location_indices[index].value,
                                               binding, format,
                                               offset_list[index].value);
  }

  template <int N>
  static constexpr auto getBindingDescription(ltl::number_t<N> n) {
    auto indexer = types.make_indexer();

    return indexer([n](auto... index) {
      return BindingDescription{n, ltl::type_v<Vertex>, stride,
                                getAttributeDescription(n.value, index)...};
    });
  }

  static constexpr auto compute_location_indices() {
    return ltl::scanl(_((x, y), x + decltype_t(y)::location_size), 0_n, types)
        .pop_back();
  }

  static constexpr auto compute_offset_list() {
    return ltl::scanl(_((x, y), x + decltype_t(y)::byte_number), 0_n, types)
        .pop_back();
  }

  static inline uint32_t stride = (... + Types::byte_number).value;
  static inline auto location_indices = compute_location_indices();
  static inline auto offset_list = compute_offset_list();
};

using Colored2DVertex = Vertex<Position2D, RgbColor, TexCoord>;

} // namespace phx

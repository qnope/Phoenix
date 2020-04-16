#pragma once
#include "../ShaderModule.h"
#include <array>
#include <ltl/ltl.h>

namespace phx {
template <typename... Shaders> class WithShaders {
  static constexpr ltl::type_list_t<Shaders...> shader_types{};
  static constexpr auto numberShaders = shader_types.length;

public:
  WithShaders(Shaders... shaders) : m_shaders{std::move(shaders)...} {
    compileTimeCheck();
  }

  auto getStages() {
    auto construct_stages = [](auto &... xs) {
      return std::array{xs.getStageInfo()...};
    };
    return m_shaders(construct_stages);
  }

private:
  void compileTimeCheck() {
    using namespace ltl;
    typed_static_assert_msg(shader_types.length > 0_n,
                            "You must have shaders within the WithShaders");

    typed_static_assert_msg(all_of_type(shader_types, is_shader_module),
                            "All arguments from WithShaders must be shaders");

    typed_static_assert_msg(
        count_type(shader_types, type_v<ShaderModule<VertexShaderType>>) <=
                1_n &&
            count_type(shader_types,
                       type_v<ShaderModule<GeometryShaderType>>) <= 1_n &&
            count_type(shader_types,
                       type_v<ShaderModule<FragmentShaderType>>) <= 1_n,
        "No multiple same shader allowed");
  }

private:
  ltl::tuple_t<Shaders...> m_shaders;
};

LTL_MAKE_IS_KIND(WithShaders, is_with_shaders, IsWithShaders, typename);

} // namespace phx

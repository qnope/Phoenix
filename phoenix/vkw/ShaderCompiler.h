#pragma once

#include <algorithm>
#include <shaderc/shaderc.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utility.h"

namespace phx {
struct ShaderErrorException {
  std::string error;
};

namespace detail {
class ShaderCompilerIncluder
    : public shaderc::CompileOptions::IncluderInterface {
public:
  shaderc_include_result *GetInclude(const char *requested,
                                     shaderc_include_type,
                                     const char *requesting, size_t) override;

  // Handles shaderc_include_result_release_fn callbacks.
  void ReleaseInclude(shaderc_include_result *result) override;

private:
  std::unordered_map<std::string, shaderc_include_result> m_result;
  std::unordered_set<std::string> m_sourceNames;
  std::unordered_map<std::string, std::string> m_contentNames;
};
} // namespace detail

class ShaderCompiler {
public:
  ShaderCompiler(bool debug) noexcept;

  std::vector<uint32_t> compile(const std::string &path,
                                shaderc_shader_kind kind) const;

private:
  shaderc::Compiler m_compiler;
  shaderc::CompileOptions m_options;
};
} // namespace phx

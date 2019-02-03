#include <string>

#include <memory>

#include "ShaderCompiler.h"
#include "utility.h"

namespace fs = std::filesystem;

namespace phx {

namespace detail {
shaderc_include_result *
ShaderCompilerIncluder::GetInclude(const char *requested, shaderc_include_type,
                                   const char *requesting, size_t) {
  const std::string baseDirectory = getBaseDirectory(requesting) + "/";
  const std::string str_requested = baseDirectory + requested;

  const std::string src = readFile(str_requested);

  shaderc_include_result *result = new shaderc_include_result;
  char *ptr = new char[str_requested.length()];
  result->source_name = ptr;
  result->source_name_length = str_requested.length();
  std::copy_n(str_requested.data(), str_requested.length(), ptr);

  ptr = new char[src.length()];
  result->content = ptr;
  result->content_length = src.length();
  std::copy_n(src.data(), src.length(), ptr);

  return result;
}

void ShaderCompilerIncluder::ReleaseInclude(shaderc_include_result *result) {
  delete[] result->content;
  delete[] result->source_name;
  delete result;
}
} // namespace detail

ShaderCompiler::ShaderCompiler(bool debug) noexcept {
  m_options.SetAutoBindUniforms(false);
  m_options.SetAutoMapLocations(false);
  m_options.SetForcedVersionProfile(450, shaderc_profile_core);

  if (debug)
    m_options.SetGenerateDebugInfo();

  if (debug)
    m_options.SetOptimizationLevel(shaderc_optimization_level_zero);

  else
    m_options.SetOptimizationLevel(shaderc_optimization_level_performance);

  m_options.SetSourceLanguage(shaderc_source_language_glsl);
  m_options.SetTargetEnvironment(shaderc_target_env_vulkan,
                                 shaderc_env_version_vulkan_1_1);
  m_options.SetWarningsAsErrors();

  m_options.SetIncluder(std::make_unique<detail::ShaderCompilerIncluder>());
}

std::vector<uint32_t> ShaderCompiler::compile(const std::string &path,
                                              shaderc_shader_kind kind) const {
  auto src = readFile(path);
  auto result = m_compiler.CompileGlslToSpv(src, kind, path.c_str(), m_options);

  if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    throw ShaderErrorException{result.GetErrorMessage()};

  return {result.begin(), result.end()};
}
} // namespace phx

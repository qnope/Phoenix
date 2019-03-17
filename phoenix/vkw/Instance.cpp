#include "Instance.h"

#include "utility.h"
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <iterator>
#include <ltl/ltl.h>
namespace phx {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  std::cerr << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

static constexpr auto createApplicationInfo() noexcept {
  vk::ApplicationInfo info;
  info.pApplicationName = "Phoenix Engine";
  info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  info.pEngineName = "Phoenix Engine";
  info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  info.apiVersion = VK_API_VERSION_1_1;
  return info;
}

static auto getExtensions(SDL_Window *window, bool debug) {
  std::vector<const char *> extensions;

  if (debug) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  uint32_t extensionsNumber;

  SDL_Vulkan_GetInstanceExtensions(window, &extensionsNumber, nullptr);

  auto baseNumberExtensions = extensions.size();
  extensions.resize(baseNumberExtensions + extensionsNumber);
  SDL_Vulkan_GetInstanceExtensions(window, &extensionsNumber,
                                   extensions.data() + baseNumberExtensions);
  auto extensionsStrings = to_string_vector(extensions);

  if (!areAvailable(extensionsStrings, extensionTag)) {
    auto notAvailables = getUnavailables(extensionsStrings, extensionTag);
    throw ExtentionInvalidException{notAvailables};
  }

  return extensions;
}

static auto getValidationLayers() {
  std::vector<const char *> layers = {"VK_LAYER_LUNARG_standard_validation"};
  auto layerStrings = to_string_vector(layers);
  if (!areAvailable(layerStrings, layerTag)) {
    auto notAvailables = getUnavailables(layerStrings, layerTag);
    throw LayerInvalidException{notAvailables};
  }

  return layers;
}

static constexpr auto createDebugMessengerInfo() noexcept {
  vk::DebugUtilsMessengerCreateInfoEXT info;
  info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

  info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
  info.pfnUserCallback = debugCallback;

  return info;
}

Instance::Instance(SDL_Window *window, bool debug) {
  constexpr auto appliInfo = createApplicationInfo();
  auto extensions = getExtensions(window, debug);

  vk::InstanceCreateInfo info;
  info.pApplicationInfo = &appliInfo;
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();

  auto layers = ::phx::getValidationLayers();

  if (debug) {
    info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    info.ppEnabledLayerNames = layers.data();
    m_validationLayers = layers;
  }

  m_handle = vk::createInstanceUnique(info);

  if (debug) {
    constexpr auto debugMessengerInfo = createDebugMessengerInfo();

    m_dispatchLoaderDynamic = vk::DispatchLoaderDynamic(*m_handle, vkGetInstanceProcAddr);
    m_debugMessenger = m_handle->createDebugUtilsMessengerEXTUnique(
        debugMessengerInfo, nullptr, m_dispatchLoaderDynamic);
  }
}

const std::vector<const char *> &Instance::getValidationLayers() const noexcept {
  return m_validationLayers;
}

} // namespace phx

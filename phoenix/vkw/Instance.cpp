#include "Instance.h"
#include <SDL2/SDL_vulkan.h>
#include <ltl/range.h>

namespace phx {
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

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

static bool isExtensionAvailable(const char *extension) {
  const auto allowedExtensions = vk::enumerateInstanceExtensionProperties();

  for (auto allowedExtension : allowedExtensions) {
    if (std::string_view{extension} == allowedExtension.extensionName)
      return true;
  }

  return false;
}

static bool isLayerAvailable(const char *layer) {
  const auto allowedLayers = vk::enumerateInstanceLayerProperties();

  for (auto allowedLayer : allowedLayers) {
    if (std::string_view{layer} == allowedLayer.layerName)
      return true;
  }

  return false;
}

static void
checkExtensionAvailability(const std::vector<const char *> &extensions) {
  for (auto extension : extensions) {
    if (!isExtensionAvailable(extension)) {
      throw ExtentionInvalidException{extension};
    }
  }
}

static void checkLayersAvailability(const std::vector<const char *> &layers) {
  for (auto layer : layers) {
    if (!isLayerAvailable(layer)) {
      throw LayerInvalidException{layer};
    }
  }
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

  checkExtensionAvailability(extensions);

  return extensions;
}

static auto getValidationLayers() {
  std::vector<const char *> validationLayers = {
      "VK_LAYER_LUNARG_standard_validation"};

  checkLayersAvailability(validationLayers);

  return validationLayers;
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

  auto layers = getValidationLayers();

  if (debug) {
    info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    info.ppEnabledLayerNames = layers.data();
    m_validationLayers = layers;
  }

  m_instance = vk::createInstanceUnique(info);

  if (debug) {
    constexpr auto debugMessengerInfo = createDebugMessengerInfo();

    m_dispatchLoaderDynamic = vk::DispatchLoaderDynamic(*m_instance);
    m_debugMessenger = m_instance->createDebugUtilsMessengerEXTUnique(
        debugMessengerInfo, nullptr, m_dispatchLoaderDynamic);
  }
}

std::vector<vk::PhysicalDevice> Instance::physicalDevices() const noexcept {
  return m_instance->enumeratePhysicalDevices();
}

const std::vector<const char *> &Instance::validationLayers() const noexcept {
  return m_validationLayers;
}
} // namespace phx

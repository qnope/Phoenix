#include "PhoenixWindow.h"

namespace phx {

static auto createWindow(Width w, Height h, WindowTitle windowTitle) {
  SDL_Window *window =
      SDL_CreateWindow(windowTitle.get().data(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, static_cast<int>(w.get()),
                       static_cast<int>(h.get()), SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    throw PhoenixWindowOpeningException{SDL_GetError()};
  }

  return window;
}

PhoenixWindow::PhoenixWindow(Width width, Height height, WindowTitle windowTitle)
    : m_width{width}, m_height{height}, m_windowHandle{
                                            createWindow(width, height, windowTitle)} {}

bool PhoenixWindow::run() const noexcept {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        return false;
    }
  }

  return true;
}

void PhoenixWindow::update() noexcept {
  vk::Device deviceHandle = m_device.getHandle();
  m_currentImageIndex = deviceHandle
                            .acquireNextImageKHR(m_swapchain.getHandle(),
                                                 std::numeric_limits<uint64_t>::max(),
                                                 *m_imgAvailable, vk::Fence())
                            .value;
}

Width PhoenixWindow::getWidth() const noexcept { return m_width; }

Height PhoenixWindow::getHeight() const noexcept { return m_height; }

Device &PhoenixWindow::getDevice() noexcept { return m_device; }

vk::AttachmentDescription PhoenixWindow::getAttachmentDescription() const noexcept {
  return m_swapchain.getAttachmentDescription();
}

void PhoenixWindow::generateFramebuffer(vk::RenderPass renderPass) noexcept {
  m_swapchain.generateFramebuffer(renderPass);
}

uint32_t PhoenixWindow::getImageCount() const noexcept {
  return m_swapchain.getImageCount();
}

uint32_t PhoenixWindow::getCurrentImageIndex() const noexcept {
  return m_currentImageIndex;
}

vk::Framebuffer PhoenixWindow::getCurrentFramebuffer() const noexcept {
  return m_swapchain.getFramebuffer(0);
}

vk::Framebuffer PhoenixWindow::getFramebuffer(uint32_t index) const noexcept {
  return m_swapchain.getFramebuffer(index);
}

vk::Semaphore PhoenixWindow::getImageAvailableSemaphore() const noexcept {
  return *m_imgAvailable;
}

vk::SwapchainKHR PhoenixWindow::getSwapchainHandle() const noexcept {
  return m_swapchain.getHandle();
}

} // namespace phx

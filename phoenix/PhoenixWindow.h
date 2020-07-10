#pragma once

#include <SDL2/SDL.h>
#include <memory>

#include "SDL_Initializer.h"
#include "constant.h"
#include "vkw/Device.h"
#include "vkw/SubpassBuilder.h"
#include "vkw/Surface.h"
#include "vkw/Swapchain.h"

namespace phx {
struct PhoenixWindowOpeningException {
    std::string exception;
};

struct SDL_WindowDeleter {
    void operator()(SDL_Window *window) { SDL_DestroyWindow(window); }
};

class PhoenixWindow {
  public:
    PhoenixWindow(Width width, Height height, WindowTitle windowTitle);

    bool run() const noexcept;

    Width getWidth() const noexcept;
    Height getHeight() const noexcept;
    float getAspect() const noexcept;

    Device &getDevice() noexcept;
    const Device &getDevice() const noexcept;

    void update() noexcept;

    vk::AttachmentDescription getAttachmentDescription() const noexcept;

    void generateFramebuffer(vk::RenderPass renderPass) noexcept;

    uint32_t getImageCount() const noexcept;
    uint32_t getCurrentImageIndex() const noexcept;
    const Framebuffer<1> &getCurrentFramebuffer() const noexcept;
    const Framebuffer<1> &getFramebuffer(uint32_t index) const noexcept;
    const std::vector<Framebuffer<1>> &getFramebuffers() const noexcept;

    vk::Semaphore getImageAvailableSemaphore() const noexcept;

    vk::SwapchainKHR getSwapchainHandle() const noexcept;

  private:
    SDL_Initializer m_sdl_initializer;
    Width m_width;
    Height m_height;

    std::unique_ptr<SDL_Window, SDL_WindowDeleter> m_windowHandle;
    SDL_Event m_event;
    Instance m_instance{m_windowHandle.get(), true};
    Surface m_surface{m_windowHandle.get(), m_instance};
    Device m_device{m_instance, m_surface};
    Swapchain m_swapchain{m_device, m_surface, m_width, m_height};
    uint32_t m_currentImageIndex;
    vk::UniqueSemaphore m_imgAvailable = m_device.createSemaphore();
};
} // namespace phx

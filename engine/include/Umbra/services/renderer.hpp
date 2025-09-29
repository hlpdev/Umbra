#pragma once

#include "Umbra/services.hpp"
#include "Umbra/engine_state.hpp"
#include "Umbra/types/data/file.hpp"
#include "Umbra/types/data/vector2.hpp"

#include <glfw/glfw3.h>
#include <imgui.h>
#include <thread>
#include <OgreRenderSystem.h>
#include <OgreRenderWindow.h>
#include <OgreStringConverter.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_THREAD_LOCALS
#include <stb_image.h>

#ifdef _WIN32
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

namespace umbra {

  class UMBRA_API RendererService final : public IService {
  public:

    RendererService(const RendererService&) = delete;
    RendererService& operator=(const RendererService&) = delete;
    RendererService(RendererService&&) = delete;
    RendererService& operator=(RendererService&&) = delete;

    explicit RendererService(EngineState* engine_state) : engine_state_(engine_state) {
      if (!glfwInit()) {
        umbra_fail("Renderer: glfw init failed");
      }

      glfwDefaultWindowHints();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      window_ = glfwCreateWindow(
        window_width_, window_height_,
        engine_state->config.name.c_str(),
        nullptr, nullptr
      );

      if (!window_) {
        umbra_fail("Renderer: glfw window creation failed");
      }

      glfwSetWindowPos(window_, window_x_, window_y_);
      glfwSetWindowUserPointer(window_, this);

      glfwSetFramebufferSizeCallback(window_, framebuffer_resize_callback);

      int framebuffer_width, framebuffer_height;
      glfwGetFramebufferSize(window_, &framebuffer_width, &framebuffer_height);
      framebuffer_width_ = static_cast<uint16_t>(framebuffer_width);
      framebuffer_height_ = static_cast<uint16_t>(framebuffer_height);

      glfwShowWindow(window_);

#ifdef _DEBUG
      auto rs_d3d11 = "RenderSystem_Direct3D11_d.dll";
      auto rs_gl3 = "RenderSystem_GL3Plus_d.dll";
      auto rs_gl = "RenderSystem_GL_d.dll";
#else
      auto rs_d3d11 = "RenderSystem_Direct3D11.dll";
      auto rs_gl3 = "RenderSystem_GL3Plus.dll";
      auto rs_gl = "RenderSystem_GL.dll";
#endif

      auto try_load = [&](const char* dll) {
        try {
          engine_state_->ogre_root->loadPlugin(dll);
          return true;
        } catch (...) {
          return false;
        }
      };

      bool loaded_any = false;
      loaded_any |= try_load(rs_d3d11);
      loaded_any |= try_load(rs_gl3);
      loaded_any |= try_load(rs_gl);

      if (!loaded_any) {
        umbra_fail("Renderer: failed to load plugin");
      }

      auto pick_render_system = [&](const char* name) -> Ogre::RenderSystem* {
        if (auto* render_system = engine_state_->ogre_root->getRenderSystemByName(name)) {
          return render_system;
        }

        return nullptr;
      };

      Ogre::RenderSystem* render_system = pick_render_system("Direct3D11 Rendering Subsystem");
      if (!render_system) {
        render_system = pick_render_system("OpenGL 3+ Rendering Subsystem");
      }
      if (!render_system) {
        render_system = pick_render_system("OpenGL Rendering Subsystem");
      }

      if (!render_system) {
        std::string avail;
        for (const Ogre::RenderSystem* renderer : engine_state_->ogre_root->getAvailableRenderers()) {
          avail += renderer->getName() + '\n';
        }
        umbra_fail("Renderer: could not find required render system. Available:\n" + avail);
      }

      engine_state_->ogre_root->setRenderSystem(render_system);
      render_system->setConfigOption("sRGB Gamma Conversion", "Yes");
      engine_state_->ogre_root->initialise(false);

      Ogre::NameValuePairList params;

#if defined(WIN32)
      HWND hwnd = glfwGetWin32Window(window_);
      if (!hwnd) {
        umbra_fail("Renderer: null hwnd from glfw");
      }
      params["externalWindowHandle"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(hwnd));
#elif defined(__linux__)
      // TODO
#elif defined (__APPLE__)
      // TODO
#endif

      ogre_window_ = engine_state_->ogre_root->createRenderWindow(
        engine_state->config.name, framebuffer_width_, framebuffer_height_, false, &params
      );

      engine_state_->ogre_scene_manager = engine_state_->ogre_root->createSceneManager();

      engine_state_->ogre_camera = engine_state->ogre_scene_manager->createCamera("Camera1");
      engine_state_->ogre_camera->setNearClipDistance(0.1f);
      engine_state_->ogre_camera->setFarClipDistance(10000.0f);
      engine_state_->ogre_camera->setAutoAspectRatio(false);
      engine_state_->ogre_camera->setAspectRatio(static_cast<Ogre::Real>(ogre_window_->getWidth()) / std::max<uint16_t>(1, ogre_window_->getHeight()));

      engine_state_->ogre_camera_node = engine_state_->ogre_scene_manager->getRootSceneNode()->createChildSceneNode("Camera1");
      engine_state_->ogre_camera_node->attachObject(engine_state_->ogre_camera);

      engine_state_->ogre_viewport = ogre_window_->addViewport(engine_state_->ogre_camera);
      engine_state_->ogre_viewport->setBackgroundColour(Ogre::ColourValue(0.05f, 0.05f, 0.08f));
    }

    const char* name() override { return "Renderer"; }

    ~RendererService() override {
      ogre_window_ = nullptr; // DANGEROUS should be freed when Ogre::Root is freed

      if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
      }

      glfwTerminate();
    }

    void show_window() const {
      glfwShowWindow(window_);
    }
    
    void hide_window() const {
      glfwHideWindow(window_);
    }

    void set_title(const std::string& title) const {
      glfwSetWindowTitle(window_, title.c_str());
    }

    void resize(const Vector2& new_size) {
      if (fullscreen_) {
        return;
      }

      window_width_ = new_size.x;
      window_height_ = new_size.y;

      glfwSetWindowSize(window_, new_size.x, new_size.y);
    }

    void set_size_limit(const Vector2& minimum, const Vector2& maximum) const {
      glfwSetWindowSizeLimits(window_, minimum.x, minimum.y, maximum.x, maximum.y);
    }

    Vector2 get_size() const {
      return Vector2(window_width_, window_height_);
    }

    void move(const Vector2& new_position) {
      if (fullscreen_) {
        return;
      }

      window_x_ = new_position.x;
      window_y_ = new_position.y;

      glfwSetWindowPos(window_, new_position.x, new_position.y);
    }

    Vector2 get_position() const {
      int x = 0, y = 0;
      glfwGetWindowPos(window_, &x, &y);

      return Vector2(x, y);
    }

    void set_fullscreen(const bool fullscreen) {
      if (fullscreen == fullscreen_) {
        return;
      }

      if (fullscreen) {
        glfwGetWindowPos(window_, &window_x_, &window_y_);
        glfwGetWindowSize(window_, &window_width_, &window_height_);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        fullscreen_ = true;
      } else {
        glfwSetWindowMonitor(window_, nullptr, window_x_, window_y_, window_width_, window_height_, 0);
        fullscreen_ = false;
      }

      if (ogre_window_) {
        ogre_window_->resize(window_width_, window_height_);
        ogre_window_->windowMovedOrResized();
      }
    }

    bool is_fullscreen() const {
      return fullscreen_;
    }

    bool should_close() const {
      return glfwWindowShouldClose(window_);
    }

    void begin_render() const {
      glfwPollEvents();
    }

    void end_render() const {
      engine_state_->ogre_root->renderOneFrame();
    }

    Vector2 get_content_scale() const {
      float sx = 1.0f, sy = 1.0f;
      glfwGetWindowContentScale(window_, &sx, &sy);

      return Vector2(sx, sy);
    }

    int get_attribute(const int attribute) const {
      return glfwGetWindowAttrib(window_, attribute);
    }

    void set_attribute(const int attribute, const int value) const {
      glfwSetWindowAttrib(window_, attribute, value);
    }

    bool set_icon(const File& icon_file) const {
      if (!engine_state_) {
        return false;
      }

      const std::vector<uint8_t> bytes = icon_file.data;

      int width = 0, height = 0, channels = 0;
      stbi_uc* pixels = stbi_load_from_memory(bytes.data(), static_cast<int>(bytes.size()), &width, &height, &channels, STBI_rgb_alpha);
      if (!pixels || width <= 0 || height <= 0) {
        if (pixels) {
          stbi_image_free(pixels);
        }

        return false;
      }

      GLFWimage image;
      image.width = width;
      image.height = height;
      image.pixels = pixels;

      glfwSetWindowIcon(window_, 1, &image);

      stbi_image_free(pixels);

      return true;
    }

    void iconify() const {
      glfwIconifyWindow(window_);
    }

    void restore() const {
      glfwRestoreWindow(window_);
    }

    void maximize() const {
      glfwMaximizeWindow(window_);
    }

    void focus() const {
      glfwFocusWindow(window_);
    }

    void request_attention() const {
      glfwRequestWindowAttention(window_);
    }

    void bind(sol::state& lua_state) {
      sol::usertype<RendererService> user_type = lua_state.new_usertype<RendererService>(name(),
        "show_window", &RendererService::show_window,
        "hide_window", &RendererService::hide_window,
        "set_title", &RendererService::set_title,
        "resize", &RendererService::resize,
        "set_size_limit", &RendererService::set_size_limit,
        "get_size", &RendererService::get_size,
        "move", &RendererService::move,
        "get_position", &RendererService::get_position,
        "set_fullscreen", &RendererService::set_fullscreen,
        "is_fullscreen", &RendererService::is_fullscreen,
        "should_close", &RendererService::should_close,
        "begin_render", &RendererService::begin_render,
        "end_render", &RendererService::end_render,
        "get_content_scale", &RendererService::get_content_scale,
        "get_attribute", &RendererService::get_attribute,
        "set_attribute", &RendererService::set_attribute,
        "set_icon", &RendererService::set_icon,
        "iconify", &RendererService::iconify,
        "restore", &RendererService::restore,
        "maximize", &RendererService::maximize,
        "focus", &RendererService::focus,
        "request_attention", &RendererService::request_attention
      );
    }

  private:
    EngineState* engine_state_ = nullptr;
    GLFWwindow* window_ = nullptr;

    Ogre::RenderWindow* ogre_window_ = nullptr;

    int window_x_ = 100, window_y_ = 100;
    int window_width_ = 1280, window_height_ = 720;

    uint16_t framebuffer_width_ = 0, framebuffer_height_ = 0;

    bool fullscreen_ = false;

    static void framebuffer_resize_callback(GLFWwindow* window, const int framebuffer_width, const int framebuffer_height) {
      auto* self = static_cast<RendererService*>(glfwGetWindowUserPointer(window));
      if (!self) {
        return;
      }

      if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        return;
      }

      self->framebuffer_width_ = framebuffer_width;
      self->framebuffer_height_ = framebuffer_height;

      if (self->ogre_window_) {
        self->ogre_window_->resize(self->framebuffer_width_, self->framebuffer_height_);
        self->ogre_window_->windowMovedOrResized();

        if (self->engine_state_->ogre_camera) {
          self->engine_state_->ogre_camera->setAspectRatio(static_cast<Ogre::Real>(self->ogre_window_->getWidth()) / std::max<uint16_t>(1, self->ogre_window_->getHeight()));
        }
      }
    }

  };

}

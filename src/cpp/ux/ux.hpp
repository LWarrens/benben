#pragma once
#ifdef _WIN32
#include <windows.h>
#endif
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>
#include <array>
#include <format>
#include "../cpp_api.hpp"
#include "../environment/Environment.hpp"

namespace VULKAN_HPP_NAMESPACE { DispatchLoaderDynamic defaultDispatchLoaderDynamic(&vkGetInstanceProcAddr); }
template<typename T>
struct ManagedHandle {
  ManagedHandle(){}
  ManagedHandle(T* handle): _handle(handle) {}
  ManagedHandle<T>& operator=(T* handle) {
    _handle=handle;
    return *this;
  }
  T* operator->() {
    return _handle;
  }
  operator T*() {
    return _handle;
  }
  virtual ~ManagedHandle();
  protected:
    T* _handle;
};

ManagedHandle<SDL_Window>::~ManagedHandle<SDL_Window>() {
  if (_handle) {
    SDL_DestroyWindow(_handle);
  }
}

typedef ManagedHandle<SDL_Window> SDLWindowHandle;

class MediaWorker : public Napi::AsyncWorker {
  std::string error_message;
  MediaWorker() = delete;
  unsigned int shaderProgram;
  unsigned int VAO;

 public:
  MediaWorker(Napi::Function& callback);
  void handle_frame(
      bool& quit,
      std::chrono::high_resolution_clock::time_point& frame_begin);
  virtual void Execute();
  virtual void Destroy();
  void OnOK();
  void OnError();
};


namespace benben {
  template<typename... Args>
  void cout_f(const char* message, Args... args) {
    std::cout << std::vformat(message, std::make_format_args(args...)) << std::endl;
  }
  template<typename... Args>
  std::runtime_error cerr_f(const char* message, Args... args) {
    return std::runtime_error(std::vformat(message, std::make_format_args(args...)));
  }
};
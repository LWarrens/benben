#include "ux.hpp"
const char* app_name = "benben_test";
const char* engine_name = "benben";

template <typename InputIt>
std::string iterable_to_list_string(InputIt begin,
                                    InputIt end,
                                    const char* separator = ", ",
                                    std::array<const char*, 2> brackets = {
                                        "[", "]"}) {
  std::stringstream ss;
  ss << brackets[0];
  if (begin < end)
    ss << *begin;
  while (++begin < end)
    ss << separator << *begin;
  ss << brackets[1];
  return ss.str();
}

MediaWorker::MediaWorker(Napi::Function& callback)
    : Napi::AsyncWorker(callback) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    throw benben::cerr_f("Couldn't initialize SDL: %s\n", SDL_GetError());
  }
}

void checkFailure(bool failing) {}

using namespace benben;
void MediaWorker::Execute() {
  SDLWindowHandle window;
  vk::Instance vk_instance;
  vk::ApplicationInfo app_info;
  vk::PhysicalDevice vk_physical_device;
  vk::DynamicLoader dl;
  bool quit = false;
  window = SDL_CreateWindow(
      app_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
      SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
          SDL_WINDOW_ALLOW_HIGHDPI);
  if (!window) {
    std::cerr << "Should create SDL window: " << SDL_GetError() << std::endl;
    return;
  }
  {
    unsigned int extension_count;
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);
    std::vector<const char*> extension_names(extension_count);
    SDL_Vulkan_GetInstanceExtensions(window, &extension_count,
                                     extension_names.data());
    std::cout << "Vulkan instance extensions: "
              << iterable_to_list_string(extension_names.begin(),
                                         extension_names.end())
                     .c_str()
              << std::endl;

    app_info = vk::ApplicationInfo(app_name, VK_MAKE_API_VERSION(1, 1, 0, 0),
                                   engine_name, VK_MAKE_API_VERSION(1, 1, 0, 0),
                                   VK_API_VERSION_1_3);
    vk::InstanceCreateInfo ic_info;
    ic_info.setEnabledExtensionCount(extension_count);
    ic_info.setPEnabledExtensionNames(extension_names);
    ic_info.setPApplicationInfo(&app_info);
    ic_info.setEnabledLayerCount(0);
    vk_instance = vk::createInstance(ic_info);
    benben::cout_f("instance created\n");
  }

  std::vector<VkPhysicalDevice> devices;
  {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());
  }
  std::cout << "instancing vk" << std::endl;
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
      dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

  // uint32_t device_count = 0;

  // if (vk::Result res =
  //         vk_instance.enumeratePhysicalDevices(&device_count, nullptr);
  //     res != vk::Result::eSuccess) {
  //   std::cerr << "Failed to enumerate physical devices" << std::endl;
  //   return;
  // }
  // std::cout << "try111" << std::endl;
  // std::vector<vk::PhysicalDevice> devices(device_count);
  // vk_instance.enumeratePhysicalDevices(&device_count, devices.data());
  // vk::PhysicalDevice current_device(nullptr);
  // if (auto it = std::find_if(devices.begin(), devices.end(),
  //                            [](auto a) { return true; });
  //     it != devices.end()) {
  //   current_device = *it;
  // } else {
  //   std::cerr << "Failed to find suitable GPUs" << std::endl;
  //   return;
  // }

  // std::cout << "try1111" << std::endl;
  // vk::PhysicalDevice current_device(nullptr);
  // if (auto it = std::find_if(devices.begin(), devices.end(),
  //                            [](auto a) { return true; });
  //     it != devices.end()) {
  //   current_device = *it;
  // } else {
  //   std::cerr << "Failed to find suitable GPUs" << std::endl;
  //   return;
  // }

  // vk::ImageCreateInfo ci(
  //     {}, vk::ImageType::e2D, vk::Format::eR8G8B8A8Unorm, {100, 100, 1}, 1,
  //     1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
  //     vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive,
  //     0, nullptr, vk::ImageLayout::eUndefined);
  SDL_Event sdl_event;
  while (!quit) {
    // wait event when I split the thread
    while (SDL_PollEvent(&sdl_event)) {
      switch (sdl_event.type) {
        case SDL_QUIT:
          quit = true;
          break;
      }
    }

  }  // End while
  SDL_Quit();
}

void MediaWorker::Destroy() {
  delete this;
}

void MediaWorker::OnOK() {
  Callback().Call({Env().Null(), Env().Null()});
}

void MediaWorker::OnError() {
  Callback().Call({Napi::String::New(Env(), error_message)});
}

Napi::Object init_ux(Napi::Env env, Napi::Object exports) {
  auto _init_scene_graph = [](const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto callback_function = info[0].As<Napi::Function>();
    auto worker = new MediaWorker(callback_function);
    worker->Queue();
    return Napi::Value();
  };
  exports.Set(Napi::String::New(env, "init_ux"),
              Napi::Function::New(env, _init_scene_graph));
  return exports;
}

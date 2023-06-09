#pragma once
#include <napi.h>
#define BENBEN_API_MODULE(name) NODE_API_MODULE(cpp_api, name)
#include "UX/ux.hpp"

Napi::Object init_ux(Napi::Env env, Napi::Object exports);
BENBEN_API_MODULE(init_ux)
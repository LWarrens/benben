add_rules("mode.debug", "mode.release")
set_languages("c++23")
set_optimize("fastest")

add_requires(
  "assimp",
  "bullet3",
  "catch2",
  "cef",
  "glew",
  "glm",
  "libsdl"
)

target("node")
  set_kind("static")
  before_link(function(target)
    if target:is_plat("windows") then
      local node_def = target:targetdir() .. '/node.def'
      local node_def_src = os.iorunv("node", { "-p", "let {node_api_symbols,js_native_api_symbols}=require('node-api-headers').symbols.v8;['LIBRARY node.exe','EXPORTS',...node_api_symbols,...js_native_api_symbols].join(String.fromCharCode(10));" })
      io.writefile(node_def, node_def_src)
      target:add("arflags", "/DEF:" .. node_def)
    end
    local node_api_headers_dir = os.iorunv("node", { "-p", "require('node-api-headers').include_dir" }):gsub("\\", "/"):gsub("\n", ""):gsub("\r", ""):gsub("\"", "")
    local node_addon_api_dir = os.iorunv("node", { "-p", "require('node-addon-api').include" }):gsub("\\", "/"):gsub("\n", ""):gsub("\r", ""):gsub("\"", "")
    target:add("includedirs", node_api_headers_dir, { public = true })
    target:add("includedirs", node_addon_api_dir, { public = true })
    target:add("linkdirs", target:targetdir(), { public = true })
    target:add("links", "node", { public = true })
  end)

target("vulkan")
  set_kind("headeronly")
  on_load(function(target)
    local vulkan_sdk_dir = os.iorunv("node", { "-p", "process.env.VULKAN_SDK ?? process.env.VULKAN_SDK_DISABLED"}):gsub("\\", "/"):gsub("\n", ""):gsub("\r", ""):gsub("\"", "")
    target:add('includedirs', vulkan_sdk_dir .. "/include", { public = true })
    target:add('linkdirs', vulkan_sdk_dir .. "/lib", { public = true })
    if target:is_plat("windows") then
      target:add('links', 'vulkan-1', { public = true })
    end
  end)

target("benben_cpp_api")
  set_extension(".node")
  add_deps("node", "vulkan")
  set_kind("shared")
  add_files(
      "src/cpp/**.cpp"
  )
  set_targetdir("src/node/")
  add_packages(
    "assimp",
    "bullet3",
    "catch2",
    "cef",
    "glew",
    "glm",
    "libsdl"
  )
  add_includedirs(
    "src/cpp/"
  )
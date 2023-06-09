# benben
A game engine with these features:
- Node.JS scripting
- Vulkan renderer
- CEF GUI system
- npm & xmake package management

## Requirements
[Node.JS](https://nodejs.org/en/download)
and
[xmake](https://xmake.io/#/getting_started)
must be installed and available in CLI

[VulkanSDK](https://vulkan.lunarg.com/sdk/home)
VULKAN_SDK env variable must be set to the sdk directory

#### Additional Windows Requirements
MSVC compiler must be installed

## Using Benben
| NPM Command | Description |
|-|-|
| `npm run build` | Build benben as a node-addon |
| `npm run buildstart` | build benben and start the initial test |
| `npm run start` | start the initial test |
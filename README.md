[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ec020ba40c7b4754ab2975940e76f178)](https://app.codacy.com/gh/DatDarkAlpaca/turnip-engine/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

# Turnip Engine
> Turnip is my favorite toy game engine. It features an unecessarily complicated RHI (a fancy way of saying it can run both Vulkan and Modern OpenGL), a suboptimal Lua engine, and several other cool things.

# Getting Started
Turnip doesn't have prebuilt binaries yet, so if you want to use it, you'll need to build it yourself (sorry).
Apart from the requirements listed below, keep in mind that this project has been only tested on [Pop!_OS 22.04 LTS](https://system76.com/pop/), using a [GeForce RTX™ 3050](https://www.nvidia.com/en-us/geforce/graphics-cards/30-series/rtx-3050/), and [driver version 580.82.09](https://www.nvidia.com/en-us/drivers/details/254126/).

Which means, your system must support: 

* VK_KHR_synchronization2
* VK_KHR_dynamic_rendering
* OpenGL 4.5: Direct Access Memory, Clip Control, and other features have been used.

You can check whether or not your graphics card supports these features [here](http://www.vulkan.gpuinfo.org/displayreport.php?id=39663#extensions).

## Prerequisites
Building this project requires the following:

* [Vulkan](https://vulkan.lunarg.com/#new_tab)
* [Lua](https://www.lua.org/download.html)
* [CMake](https://cmake.org/download/)

You may need to set these environment variables if they aren't automatically detected:
1. **Vulkan**:
    Set `VULKAN_SDK` to point to your Vulkan SDK installation (e.g., C:/Vulkan/<version>).

If the SDK fails to include it automatically, you must provide an environment variable named VULKAN_SDK under `VulkanSDK/<version>`.

## Installation

1. **Clone the repository**
    ```bash
    git clone --recurse-submodules https://github.com/dat-alpaca/turnip-engine
    ```

    In case you forget the `--recurse-submodules` option, just run `git submodule update --init --recursive`.

2. **Generate the project**
    ```bash
    cmake -G Ninja -S . -B build/turnip -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    ```
    Some other options can be set, such as:
    | Option Name             | Information                              | Default |
    | -------------           | -------------                            | :---:   |
    | -DTUR_BUILD_SANDBOX     | Builds the turnip sandbox program        | ON      |
    | -DTUR_BUILD_ENGINE      | Builds the turnip engine library         | ON      |
    | -DTUR_USE_GLFW*         | Uses the GLFW windowing implementation   | ON      |
    | -DTUR_USE_OPENGL        | Uses the OpenGL backend                  | ON      |
    | -DTUR_USE_VULKAN        | Uses the Vulkan backend                  | OFF     |

   \* _This option is a lie. Turnip doesn't support other platforms yet._

4. **Compile the project**
   Use the generated project files to build the project.
   This repository uses the `cmake --build build/turnip --parallel 12` command officially.  
   However, if you're using a linux distribution, you can run `make` directly.
   For Windows users, use the `Build All` option upon opening the Visual Studio solution project, or run `msbuild`.
   
   After that, you should have a working copy under `turnip-engine/build/turnip/`.


## Configuration
The engine requires a location file specified as its main constructor argument. A default file can be located under [`res/engine_config.json`](https://github.com/dat-alpaca/turnip-engine/blob/main/res/engine_config.json).

> [!WARNING]
> Changing the graphics API string (`windowingCapabilities/api`) will not change the actual graphics API used.
As the TurnipRHI system uses static polymorphism, the engine has to be recompiled with the correct arguments.
Check the options listed [here](#installation) for more information. 

# License

This project is licensed under the [MPL-2.0 license](https://www.mozilla.org/en-US/MPL/2.0/) - see the [LICENSE](LICENSE) file for details. It is important to note that until [a certain commit](https://github.com/dat-alpaca/turnip-engine/commit/3e2776747c4ac76e5793da7dedc50eb300a1aa40), the project was licensed under the [MIT License](https://opensource.org/license/mit).
